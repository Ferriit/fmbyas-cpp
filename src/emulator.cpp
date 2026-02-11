#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <cstdint>
#include <iostream>

#define PC registers[0]
#define SP registers[1]
#define FLAGS registers[2]

const int regAmount = 16;


namespace opcode {
    constexpr int NONE = 0;
    constexpr int REG = 1;
    constexpr int VAL = 2;

    const std::vector<std::string> opcodes {
        "ldi", "mov", "ld", "str", "xchg",
        "psh", "pshi", "pop", "pek", "srmv",
        "swp", "lea",
        "add", "sub", "mul", "div", "inc", "dec",
        "and", "or", "not", "xor",
        "shl", "shr", "rsl", "rsr",
        "cmp", "jmp", "jz", "jnz", "jgt", "jlt", "jge", "jle",
        "call", "callr", "ret",
        "nop", "hlt", "wait", "waiti", "cont"
    };

    enum class eOpcode : uint8_t {
        LDI, MOV, LD, STR, XCHG,
        PSH, PSHI, POP, PEK, SRMV,
        SWP, LEA,
        ADD, SUB, MUL, DIV, INC, DEC,
        AND, OR, NOT, XOR,
        SHL, SHR, RSL, RSR,
        CMP, JMP, JZ, JNZ, JGT, JLT, JGE, JLE,
        CALL, CALLR, RET,
        NOP, HLT, WAIT, WAITI, CONT
    };
  
    const std::unordered_map<std::string, std::vector<int>> operands {
        {"ldi",   {REG, VAL}},
        {"mov",   {REG, REG}},
        {"ld",    {REG, VAL}},
        {"str",   {VAL, REG}},
        {"xchg",  {REG, REG}},
        {"psh",   {REG, NONE}},
        {"pshi",  {VAL, NONE}},
        {"pop",   {REG, NONE}},
        {"pek",   {REG, NONE}},
        {"srmv",  {NONE, NONE}},
        {"swp",   {VAL, REG}},
        {"lea",   {REG, VAL}},
        {"add",   {REG, REG}},
        {"sub",   {REG, REG}},
        {"mul",   {REG, REG}},
        {"div",   {REG, REG}},
        {"inc",   {REG, NONE}},
        {"dec",   {REG, NONE}},
        {"and",   {REG, REG}},
        {"or",    {REG, REG}},
        {"not",   {REG, NONE}},
        {"xor",   {REG, REG}},
        {"shl",   {REG, VAL}},
        {"shr",   {REG, VAL}},
        {"rsl",   {REG, VAL}},
        {"rsr",   {REG, VAL}},
        {"cmp",   {REG, REG}},
        {"jmp",   {VAL, NONE}},
        {"jz",    {VAL, NONE}},
        {"jnz",   {VAL, NONE}},
        {"jgt",   {VAL, NONE}},
        {"jlt",   {VAL, NONE}},
        {"jge",   {VAL, NONE}},
        {"jle",   {VAL, NONE}},
        {"call",  {VAL, NONE}},
        {"callr", {REG, NONE}},
        {"ret",   {NONE, NONE}},
        {"nop",   {NONE, NONE}},
        {"hlt",   {NONE, NONE}},
        {"wait",  {REG, NONE}},
        {"waiti", {VAL, NONE}},
        {"cont",  {NONE, NONE}}
    };
}

enum FlagBit {
    ZERO_BIT = 0,    // Set if result is 0
    CARRY_BIT = 1,   // Set if unsigned overflow occurs
    SIGN_BIT = 2,    // Set if result is negative (MSB is 1)
    OVERFLOW_BIT = 3 // Set if signed overflow occurs
};


std::vector<uint8_t> readBytes(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("failed to open file");

    file.seekg(0, std::ios::end);
    std::size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    file.read(reinterpret_cast<char*>(buffer.data()), size);

    return buffer;
}


int to_int(const std::string& s) {
    int value = 0;
    for (char c : s) {
        if (!std::isdigit(static_cast<unsigned char>(c)))
            return -1;
        value = value * 10 + (c - '0');
    }
    return value;
}


std::string hex(int n) {
    const char* digits = "0123456789ABCDEF";

    if (n == 0)
        return "0x00";

    unsigned int x = static_cast<unsigned int>(n);
    std::string raw;

    while (x > 0) {
        raw.push_back(digits[x & 0xF]);
        x >>= 4;
    }

    std::reverse(raw.begin(), raw.end());

    if (raw.size() % 2 != 0)
        raw.insert(raw.begin(), '0');

    std::string out;
    for (size_t i = 0; i < raw.size(); i += 2) {
        if (!out.empty())
            out += " ";
        out += "0x";
        out += raw[i];
        out += raw[i + 1];
    }

    return out;
}

std::string registerName(int idx) {
    if (idx == 0) return "pc";
    if (idx == 1) return "stackptr";
    if (idx == 2) return "flags";
    if (idx >= 3 && idx <= 10) return "io" + std::to_string(idx - 3);
    if (idx >= 11 && idx < 11 + regAmount) return "r" + std::to_string(idx - 10);
    return "???"; // invalid / does not exist
}


void setFlag(std::vector<uint16_t>& regs, FlagBit bit, bool condition) {
    if (condition) {
        regs[2] |= (1 << bit);  // Set bit to 1
    } else {
        regs[2] &= ~(1 << bit); // Clear bit to 0
    }
}

bool getFlag(std::vector<uint16_t>& regs, FlagBit bit) {
    return (regs[2] >> bit) & 1;
}


int runProgram(std::vector<uint8_t> bytes, std::vector<uint16_t>& registers, std::vector<uint8_t>& mem) {
    bool running = true;

    std::copy(bytes.begin(), bytes.end(), mem.begin());

    int freeze = 0;

    PC = 0;
    SP = 65535; // memory amount - 2 bytes
    while (running) {
        uint8_t b = mem[PC];
        if (freeze > 0) {
            freeze--;
            goto skip;
        }

        switch (b) {
            case (uint8_t)(opcode::eOpcode::LDI): {
                int reg = (mem[PC + 1] << 8) | mem[PC + 2];
                int val = (mem[PC + 3] << 8) | mem[PC + 4];
                registers[reg] = val;
                break;
            }
            case (uint8_t)(opcode::eOpcode::MOV): {
                int regB = (mem[PC + 1] << 8) | mem[PC + 2];
                int regA = (mem[PC + 3] << 8) | mem[PC + 4];
                registers[regA] = registers[regB];
                break;
            }
            case (uint8_t)(opcode::eOpcode::LD): {
                int reg = (mem[PC + 1] << 8) | mem[PC + 2];
                int addr = (mem[PC + 3] << 8) | mem[PC + 4];
                registers[reg] = (mem[addr] << 8) | mem[addr + 1];
                break;
            }
            case (uint8_t)(opcode::eOpcode::STR): {
                int addr = (mem[PC + 1] << 8) | mem[PC + 2];
                int reg = (mem[PC + 3] << 8) | mem[PC + 4];
                mem[addr] = (registers[reg] & 0xFF00) >> 8;
                mem[addr + 1] = registers[reg] &0x00FF;
                break;
            }
            case (uint8_t)(opcode::eOpcode::XCHG): {
                int regB = (mem[PC + 1] << 8) | mem[PC + 2];
                int regA = (mem[PC + 3] << 8) | mem[PC + 4];

                uint16_t regI = registers[regB];
                registers[regB] = registers[regA];
                registers[regA] = regI;
                break;
            }
            case (uint8_t)(opcode::eOpcode::PSH): {
                int reg = (mem[PC + 1] << 8) | mem[PC + 2];
                mem[SP]     = registers[reg] & 0x00FF;
                mem[SP - 1] = (registers[reg] & 0xFF00) >> 8;
                SP -= 2;
                break;
            }
            case (uint8_t)(opcode::eOpcode::PSHI): {
                int val = (mem[PC + 1] << 8) | mem[PC + 2];
                mem[SP] = val & 0x00FF;
                mem[SP - 1] = (registers[val] & 0xFF00) >> 8;
                SP -= 2;
                break;
            }
            case (uint8_t)(opcode::eOpcode::POP): {
                int reg = (mem[PC + 1] << 8) | mem[PC + 2];
                registers[reg] = (mem[SP + 1] << 8) | mem[SP + 2];
                mem[SP + 1] = 0;
                mem[SP + 2] = 0;
                SP += 2;
                break;
            }
            case (uint8_t)(opcode::eOpcode::PEK): {
                int reg = (mem[PC + 1] << 8) | mem[PC + 2];
                registers[reg] = (mem[SP + 1] << 8) | mem[SP + 2];
                SP += 2;
                break;
            }
            case (uint8_t)(opcode::eOpcode::SRMV): {
                mem[SP + 1] = 0;
                mem[SP + 2] = 0;
                SP += 2;
                break;
            }
            case (uint8_t)(opcode::eOpcode::SWP): {
                int addr = (mem[PC + 1] << 8) | mem[PC + 2];
                int reg = (mem[PC + 3] << 8) | mem[PC + 4];

                uint16_t regI = registers[reg];
                registers[reg] = mem[addr];
                mem[addr] = regI;
                break;
            }
            case (uint8_t)(opcode::eOpcode::LEA): {
                int reg = (mem[PC + 1] << 8) | mem[PC + 2];
                int addr = (mem[PC + 3] << 8) | mem[PC + 4];

                registers[reg] = addr;
                break;
            }
            case (uint8_t)(opcode::eOpcode::ADD): {
                int regA = (mem[PC + 1] << 8) | mem[PC + 2];
                int regB = (mem[PC + 3] << 8) | mem[PC + 4];

                registers[regA] = registers[regA] + registers[regB];
                break;
            }
            case (uint8_t)(opcode::eOpcode::SUB): {
                int regA = (mem[PC + 1] << 8) | mem[PC + 2];
                int regB = (mem[PC + 3] << 8) | mem[PC + 4];

                registers[regA] = registers[regA] - registers[regB];
                break;
            }
            case (uint8_t)(opcode::eOpcode::MUL): {
                int regA = (mem[PC + 1] << 8) | mem[PC + 2];
                int regB = (mem[PC + 3] << 8) | mem[PC + 4];

                registers[regA] = registers[regA] * registers[regB];
                break;
            }
            case (uint8_t)(opcode::eOpcode::DIV): {
                int regA = (mem[PC + 1] << 8) | mem[PC + 2];
                int regB = (mem[PC + 3] << 8) | mem[PC + 4];

                if (regB != 0) {
                    registers[regA] = registers[regA] / registers[regB];
                }
                else {
                    registers[regA] = 0;
                }
                break;
            }
            case (uint8_t)(opcode::eOpcode::INC): {
                int reg = (mem[PC + 1] << 8) | mem[PC + 2];
                
                registers[reg]++;
                break;
            }
            case (uint8_t)(opcode::eOpcode::DEC): {
                int reg = (mem[PC + 1] << 8) | mem[PC + 2];
                
                registers[reg]--;
                break;
            }
            case (uint8_t)(opcode::eOpcode::AND): {
                int regA = (mem[PC + 1] << 8) | mem[PC + 2];
                int regB = (mem[PC + 3] << 8) | mem[PC + 4];

                registers[regA] = registers[regA] & registers[regB];
                break;
            }
            case (uint8_t)(opcode::eOpcode::OR): {
                int regA = (mem[PC + 1] << 8) | mem[PC + 2];
                int regB = (mem[PC + 3] << 8) | mem[PC + 4];

                registers[regA] = registers[regA] | registers[regB];
                break;
            }
            case (uint8_t)(opcode::eOpcode::NOT): {
                int reg = (mem[PC + 1] << 8) | mem[PC + 2];
                
                registers[reg] = !registers[reg];
                break;
            }
            case (uint8_t)(opcode::eOpcode::XOR): {
                int regA = (mem[PC + 1] << 8) | mem[PC + 2];
                int regB = (mem[PC + 3] << 8) | mem[PC + 4];

                registers[regA] = registers[regA] ^ registers[regB];
                break;
            }
            case (uint8_t)(opcode::eOpcode::SHL): {
                int regA = (mem[PC + 1] << 8) | mem[PC + 2];
                int regB = (mem[PC + 3] << 8) | mem[PC + 4];

                registers[regA] = registers[regA] << registers[regB];
                break;
            }
            case (uint8_t)(opcode::eOpcode::SHR): {
                int regA = (mem[PC + 1] << 8) | mem[PC + 2];
                int regB = (mem[PC + 3] << 8) | mem[PC + 4];

                registers[regA] = registers[regA] >> registers[regB];
                break;
            }
            case (uint8_t)(opcode::eOpcode::RSL): {
                int regA = (mem[PC + 1] << 8) | mem[PC + 2];
                int regB = (mem[PC + 3] << 8) | mem[PC + 4];

                uint16_t val = registers[regA];
                uint16_t shift = registers[regB] % 16;
                
                if (shift == 0) {
                    registers[regA] = val;
                } else {
                    registers[regA] = (val << shift) | (val >> (16 - shift));
                }
                PC += 5;
                break;
            }
            case (uint8_t)(opcode::eOpcode::RSR): {
                int regA = (mem[PC + 1] << 8) | mem[PC + 2];
                int regB = (mem[PC + 3] << 8) | mem[PC + 4];

                uint16_t val = registers[regA];
                uint16_t shift = registers[regB] % 16;

                if (shift == 0) {
                    registers[regA] = val;
                } else {
                    registers[regA] = (val >> shift) | (val << (16 - shift));
                }
                PC += 5;
                break;
            }
            case (uint8_t)(opcode::eOpcode::CMP): {
                int regA = (mem[PC + 1] << 8) | mem[PC + 2];
                int regB = (mem[PC + 3] << 8) | mem[PC + 4];

                uint16_t valA = registers[regA];
                uint16_t valB = registers[regB];
                uint16_t result = valA - valB;

                setFlag(registers, ZERO_BIT, (result == 0));
                setFlag(registers, SIGN_BIT, (result & 0x8000)); // Check 15th bit
                setFlag(registers, CARRY_BIT, (valA < valB));    // Borrow occurred
                break;
            }
            case (uint8_t)(opcode::eOpcode::JMP): {
                int val = (mem[PC + 1] << 8) | mem[PC + 2];

                PC = val - 5;
                break;
            }
            case (uint8_t)(opcode::eOpcode::JZ): {
                if (getFlag(registers, FlagBit::ZERO_BIT)) {
                    int val = (mem[PC + 1] << 8) | mem[PC + 2];

                    PC = val - 5;
                }
                break;
            }
            case (uint8_t)(opcode::eOpcode::JNZ): {
                if (!getFlag(registers, FlagBit::ZERO_BIT)) {
                    int val = (mem[PC + 1] << 8) | mem[PC + 2];

                    PC = val - 5;
                }
                break;
            }
            case (uint8_t)(opcode::eOpcode::JGT): {
                if (!getFlag(registers, FlagBit::SIGN_BIT) && !getFlag(registers, FlagBit::ZERO_BIT)) {
                    int val = (mem[PC + 1] << 8) | mem[PC + 2];

                    PC = val - 5;
                }
                break;
            }
            case (uint8_t)(opcode::eOpcode::JLT): {
                if (getFlag(registers, FlagBit::SIGN_BIT)) {
                    int val = (mem[PC + 1] << 8) | mem[PC + 2];

                    PC = val - 5;
                }
                break;
            }
            case (uint8_t)(opcode::eOpcode::JGE): {
                if (!getFlag(registers, FlagBit::SIGN_BIT)) {
                    int val = (mem[PC + 1] << 8) | mem[PC + 2];

                    PC = val - 5;
                }
                break;
            }
            case (uint8_t)(opcode::eOpcode::JLE): {
                if (getFlag(registers, FlagBit::SIGN_BIT) && getFlag(registers, FlagBit::ZERO_BIT)) {
                    int val = (mem[PC + 1] << 8) | mem[PC + 2];

                    PC = val - 5;
                }
                break;
            }
            case (uint8_t)(opcode::eOpcode::NOP): {
                break;
            }
            case (uint8_t)(opcode::eOpcode::HLT): {
                running = false;
            }
            case (uint8_t)(opcode::eOpcode::WAIT): {
                int reg = (mem[PC + 1] << 8) | mem[PC + 2];
                
                freeze = registers[reg];
                break;
            }
            case (uint8_t)(opcode::eOpcode::WAITI): {
                int val = (mem[PC + 1] << 8) | mem[PC + 2];
                
                freeze = val;
                break;
            }
        }
        PC += 5;
        skip:
    }
    return 0;
}

int main(int argc, char** argv) {
    std::vector<uint8_t> bytes = readBytes("out.bin");

    std::vector<uint16_t> registers(10 + regAmount, 0);
    std::vector<uint8_t> memory(65536);

    std::string OP = "";

    int i = 0;
    for (uint8_t b: bytes) {
        std::cout << hex(b);
        if (i % 5 == 0) {
            OP = opcode::opcodes[b];
            std::cout << "\x1b[38;5;231;1m\t" << opcode::opcodes[b] << "\x1b[0m";
        }
        else if (opcode::operands.at(OP)[(i % 3) - 1] == opcode::REG) {
            std::cout << "\x1b[38;5;204m\t" << registerName(b) << "\x1b[0m";
        }
        else if (opcode::operands.at(OP)[(i % 3) - 1] == opcode::VAL) {
            std::cout << "\x1b[38;5;214m\t" << (int)b << " (" << (char)b << ")\x1b[0m";
        }
        else {
            std::cout << "\x1b[38;5;232;48;5;252;1m\tnil\x1b[0m";
        }

        std::cout << std::endl;
    
        i++;
    }

    runProgram(bytes, registers, memory);
    std::cout << registers[0x03] << std::endl;

    return 0;
}

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <algorithm>
#include <iterator>
#include <cstdint>


namespace opcode {
    constexpr int NONE = 0;
    constexpr int REG = 1;
    constexpr int VAL = 2;

    const std::vector<std::string> opcodes {
        "ldi", "mov", "ld", "str", "rld", "rstr", "xchg",
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
        LDI, MOV, LD, STR, RLD, RSTR, XCHG,
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
        {"rld", {REG, REG}},
        {"rstr", {REG, REG}},
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

int regAmount = 16;


using byte = uint8_t;

void emit8(std::vector<uint8_t>& out, uint8_t v) {
    out.push_back(v);
}

void emit16_be(std::vector<uint8_t>& out, uint16_t value) {
    out.push_back((value >> 8) & 0xFF); // high byte
    out.push_back(value & 0xFF);        // low byte
}

void pad_to_4(std::vector<byte>& out, size_t instr_start) {
    while (out.size() - instr_start < 4)
        out.push_back(0x00);
}


std::string removeComments(std::string code) {
    bool isComment = false;

    std::string out;
    for (char c: code) {
        if (!isComment && c != ';' && c != '\n' && c != ',') {
            out += c;
        }
        else if (c == ';') {
            isComment = true;
        }
        else if (c == '\n') {
            isComment = false;
            out += '\n';
        }
    }

    return out;
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

int parseRegisters(std::string regName) {
    if (regName == "pc") return 0;
    if (regName == "sp") return 1;
    if (regName == "flags") return 2;

    // Handle io0 - io7 (Indices 3 - 10)
    if (regName.size() >= 3 && regName.substr(0, 2) == "io") {
        try {
            int num = std::stoi(regName.substr(2));
            if (num >= 0 && num <= 7) {
                return num + 3; // Offset by 3 (pc, sp, flags)
            }
        } catch (...) {}
        std::cerr << "Invalid IO register: " << regName << std::endl;
        return -1;
    }

    // Handle r0 - rn (Indices 11 - n+11)
    if (regName.size() >= 2 && regName[0] == 'r') {
        try {
            int num = std::stoi(regName.substr(1));
            int idx = num + 11; // Offset by 11 (pc, sp, flags, 8 io regs)
            if (num >= 0 && idx < 11 + regAmount) {
                return idx;
            }
        } catch (...) {}
        std::cerr << "Invalid register: " << regName << std::endl;
        return -1;
    }

    return -1;
}

bool is_integer(const std::string& s) {
    if (s.empty()) return false;

    size_t i = 0;
    if (s[0] == '-' || s[0] == '+') i = 1;

    for (; i < s.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(s[i])))
            return false;
    }
    return true;
}

auto parseNumber = [](const std::string &s) -> uint16_t {
    try {
        if (s.size() > 2 && s[0] == '0') {
            if (s[1] == 'x' || s[1] == 'X') {
                return static_cast<uint16_t>(std::stoul(s, nullptr, 16)); // hex
            } else if (s[1] == 'b' || s[1] == 'B') {
                return static_cast<uint16_t>(std::stoul(s.substr(2), nullptr, 2)); // binary
            }
        }
        return static_cast<uint16_t>(std::stoul(s, nullptr, 10)); // decimal
    } catch (...) {
        return 0;
    }
};

std::vector<uint8_t> tokenize(const std::vector<std::string>& code) {
    std::unordered_map<std::string, uint16_t> labels;
    std::vector<uint8_t> out;

    auto is_opcode = [](const std::string &s) {
        return std::find(opcode::opcodes.begin(), opcode::opcodes.end(), s) != opcode::opcodes.end();
    };

    int instr_count = 0;
    for (const auto& tok : code) {
        if (tok.empty()) continue;
        if (tok.back() == ':') {
            labels[tok.substr(0, tok.size() - 1)] = static_cast<uint16_t>(instr_count * 5);
        } else if (is_opcode(tok)) {
            instr_count++;
        }
    }

    for (size_t i = 0; i < code.size(); ++i) {
        const std::string &tok = code[i];
        if (tok.empty() || tok.back() == ':') continue;

        if (is_opcode(tok)) {
            uint8_t op_idx = static_cast<uint8_t>(std::distance(opcode::opcodes.begin(),
                                                  std::find(opcode::opcodes.begin(), opcode::opcodes.end(), tok)));
            out.push_back(op_idx);

            const std::vector<int>& requirements = opcode::operands.at(tok);
            uint16_t ops[2] = {0, 0};

            for (int step = 0; step < 2; ++step) {
                if (requirements[step] != opcode::NONE) {
                    i++; 
                    if (i < code.size()) {
                        const std::string &operand_tok = code[i];
                        
                        int reg = parseRegisters(operand_tok);
                        if (reg != -1) {
                            ops[step] = static_cast<uint16_t>(reg);
                        } else if (labels.count(operand_tok)) {
                            ops[step] = labels[operand_tok];
                        } else {
                            try {
                                ops[step] = static_cast<uint16_t>(parseNumber(operand_tok));
                            } catch (...) { ops[step] = 0; }
                        }
                    }
                } else {
                    ops[step] = 0;
                }
            }

            emit16_be(out, ops[0]);
            emit16_be(out, ops[1]);
        }
    }
    return out;
}


std::vector<std::string> splitCode(std::string code) {
    std::vector<std::string> result;
    std::string curr;

    for (char c: code) {
        if (c != '\n' && c != ' ') {
            curr += c;
        }

        else {
            if (curr != "") {
                result.push_back(curr);
            }
            curr = "";
        }
    }

    return result;
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


int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Invalid amount of arguments, file name is required!" << std::endl;
        return 1;
    }

    std::ifstream File(argv[1]);
    std::string buf;
    std::string text;

    while (std::getline(File, text)) {
        buf += text + "\n";
    }

    std::vector<std::string> temp = splitCode(removeComments(buf));

    std::vector<uint8_t> result = tokenize(temp);

    std::ofstream outstream("out.bin", std::ios::binary);
    outstream.write(reinterpret_cast<const char*>(result.data()), result.size());

    outstream.close();

    for (int i: result) {
        std::cout << hex(i) << " ";
    }

    std::cout << std::endl;

    return 1;
}

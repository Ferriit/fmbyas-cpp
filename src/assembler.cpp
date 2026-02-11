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
};

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
    // pc:          0
    // stackptr:    1
    // io0 - io7:   2 - 9
    // r0 - rn:     10 - n + 10
    // n = max(regAmount, 245)

    if (regName == "pc") {
        return 0;
    }

    if (regName == "stackptr") {
        return 1;
    }

    if (regName.substr(0, 2) == "io") {
        if (regName.size() > 3) {
            std::cerr << "Invalid register name: '" << regName << "'" << std::endl;
            return -1;
        }
        std::string regStr(1, regName[2]);
        int num = to_int(regStr) + 2;
        if (num > 9) {
            std::cerr << "Invalid register name: '" << regName << "'" << std::endl;
            return -1;
        }
        return num;
    }
    if (regName[0] == 'r') {
        std::string regStr = regName.substr(1); // remove 'r'
        int num = to_int(regStr) + 10;
    
        if (num < 10 || num >= regAmount + 10)
            return -1;
    
        return num;
    }

    //std::cerr << "Invalid register name: '" << regName << "'" << std::endl;
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

std::vector<uint8_t> tokenize(const std::vector<std::string>& code) {
    std::unordered_map<std::string, uint16_t> labels;
    std::vector<uint8_t> out;

    auto is_opcode = [](const std::string &s) {
        return std::find(opcode::opcodes.begin(), opcode::opcodes.end(), s) != opcode::opcodes.end();
    };

    int instr_count = 0;
    for (size_t i = 0; i < code.size(); ++i) {
        const std::string &tok = code[i];
        if (!tok.empty() && tok.back() == ':') {
            labels[tok.substr(0, tok.size() - 1)] = static_cast<uint16_t>(instr_count * 5);
        } else if (is_opcode(tok)) {
            ++instr_count;
        }
    }

    for (size_t i = 0; i < code.size(); ) {
        const std::string &tok = code[i];
        if (!tok.empty() && tok.back() == ':') { ++i; continue; }
        if (!is_opcode(tok)) { ++i; continue; }

        // Write Opcode (1 byte)
        uint8_t op_idx = static_cast<uint8_t>(std::distance(opcode::opcodes.begin(),
                                              std::find(opcode::opcodes.begin(), opcode::opcodes.end(), tok)));
        out.push_back(op_idx);

        int32_t op1 = 0, op2 = 0;
        size_t j = i + 1;
        int taken = 0;

        while (j < code.size() && taken < 2) {
            const std::string &t = code[j];
            if (!t.empty() && t.back() == ':') break;
            if (is_opcode(t)) break;

            // Try Register
            int reg = parseRegisters(t);
            if (reg != -1) {
                if (taken == 0) op1 = reg; else op2 = reg;
            } 
            // Try Label
            else if (labels.count(t)) {
                if (taken == 0) op1 = labels[t]; else op2 = labels[t];
            }
            // Try Immediate
            else if (is_integer(t)) {
                int v = std::stoi(t);
                if (taken == 0) op1 = v; else op2 = v;
            }

            taken++;
            j++;
        }

        emit16_be(out, static_cast<uint16_t>(op1));
        emit16_be(out, static_cast<uint16_t>(op2));

        i = j;
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

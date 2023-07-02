/*
    Starting skeleton was provided by Dr. Mohamed Shalan.

    References:
    (1) The risc-v ISA Manual ver. 2.1 @ https://riscv.org/specifications/
    (2) https://github.com/michaeljclark/riscv-meta/blob/master/meta/opcodes
*/

/*
    To-Do:
    I_imm (check)
    S_imm
    SB_imm
    U_imm
    UJ_imm

    compressed instructions support

*/

#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <iomanip>
// debugging: remove the line below when debugging is finished
#include <bitset>

using namespace std;

unsigned int pc;
unsigned char memory[(16 + 64) * 1024];
unsigned int reg[32];

void emitError(char *s)
{
    cout << s;
    exit(0);
}

void printPrefix(unsigned int instA, unsigned int instW)
{
    cout << "0x" << hex << std::setfill('0') << std::setw(8) << instA << "\t0x" << std::setw(8) << instW;
}

void instDecExec(unsigned int instWord)
{
    unsigned int rd, rs1, rs2, funct3, funct7, opcode;
    unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
    unsigned int address;

    unsigned int instPC = pc - 4;

    opcode = instWord & 0x0000007F;
    rd = (instWord >> 7) & 0x0000001F;
    funct3 = (instWord >> 12) & 0x00000007;
    rs1 = (instWord >> 15) & 0x0000001F;
    rs2 = (instWord >> 20) & 0x0000001F;
    funct7 = (instWord >> 25) & 0x0000007F;

    // â€” inst[31] â€” inst[30:25] inst[24:21] inst[20]
    I_imm = ((instWord >> 20) & 0x7FF) | (((instWord >> 31) ? 0xFFFFF800 : 0x0)); // why the ORing?

    printPrefix(instPC, instWord);

    if (opcode == 0x33 || opcode == 0x3B)
    { // R Instructions
        // debugging: check if opcode == 0x3B is unsupported (addw, subw, others)

        switch (funct3)
        {
        case 0x00:
            if (funct7 == 0x00)
            {
                cout << "\tADD\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                reg[rd] = reg[rs1] + reg[rs2];
            }
            else if (funct7 == 0x20)
            {
                cout << "\tSUB\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                reg[rd] = reg[rs1] - reg[rs2];
            }
            break;

        case 0x4:
            if (funct7 == 0x00)
            {
                cout << "\tXOR\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                reg[rd] = reg[rs1] ^ reg[rs2];
            }
            break;

        case 0x6:
            if (funct7 == 0x00)
            {
                cout << "\tOR\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                reg[rd] = reg[rs1] | reg[rs2];
            }
            break;

        case 0x7:
            if (funct7 == 0x00)
            {
                cout << "\tAND\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                reg[rd] = reg[rs1] & reg[rs2];
            }
            break;

        case 0x1:
            if (funct7 == 0x00)
            {
                cout << "\tSLL\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                reg[rd] = rs1 << rs2;
            }
            break;

        case 0x5:
            if (funct7 == 0x00)
            {
                cout << "\tSRL\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                reg[rd] = rs1 >> rs2;
            }
            else if (funct7 == 0x20)
            {
                cout << "\tSRA\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                unsigned int temp = rs2;

                bool isNeg = reg[rs1] & 0x80000000;
                reg[rd] = rs1 >> rs2;

                // debugging: could rs2 be negative? Should we consider the 2's complement?

                for (unsigned int i = 0; i < temp; i++)
                {
                    reg[rd] = reg[rd] | (isNeg << 31 - i);
                }
            }
            break;

        case 0x2:
            if (funct7 == 0x00)
            {
                cout << "\tSLT\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                // debugging: could rs1 or rs2 be negative? Should we consider the 2's complement?
                if (reg[rs1] < reg[rs2])
                {
                    reg[rd] = 0b1;
                }
                else
                {
                    reg[rd] = 0b0;
                }
            }
            break;

        case 0x3:
            if (funct7 == 0x00)
            {
                cout << "\tSLTU\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                if (reg[rs1] < reg[rs2])
                {
                    reg[rd] = 0b1;
                }
                else
                {
                    reg[rd] = 0b0;
                }
            }
            break;

        default:
            cout << "\tUnkown R Instruction \n";
        }
    }
    else if (opcode == 0x13 || opcode == 0x03 || opcode == 0x67 || opcode == 0x73 || opcode == 0x0F || opcode == 0x1B)
    { // I instructions
        // debugging: check if opcode == 0x0F is unsupported (fence, or other)
        // debugging: check if opcode == 0x1B is unsupported (addiw, slliw, or others)

        switch (funct3)
        {
        case 0:
            cout << "\tADDI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
            reg[rd] = reg[rs1] + (int)I_imm;
            // debugging: check the type cast (int)I_imm, and check for negative immediates
            break;
        default:
            cout << "\tUnkown I Instruction \n";
        }
    }
    else if (opcode == 0x23)
    {
        // S instructions
    }
    else if (opcode == 0x63)
    {
        // SB instructions
    }
    else if (opcode == 0x6F)
    {
        // UJ instructions
    }
    else if (opcode == 0x37 || opcode == 0x17)
    {
        // U instructions
    }
    else
    {
        cout << "\tUnkown Instruction Type \n";
    }
}

int main(int argc, char *argv[])
{

    unsigned int instWord = 0;
    ifstream inFile;
    ofstream outFile;

    /*
        if (argc < 1)
            emitError("use: rvcdiss <machine_code_file_name>\n");

        inFile.open(argv[1], ios::in | ios::binary | ios::ate);
    */

    // debugging: remove the line below and uncomment the block above when debugging is finished
    inFile.open("machineCode.bin", ios::in | ios::binary | ios::ate);

    /*
        debugging: remove this comment block when debugging is finished

        instructions currently in machineCode.bin:
        add x8, x8, x9
        srl x8, x8, x9

        To modify machineCode.bin, use a hex editor or bin editor, write in hex and in little endian.

    */

    if (inFile.is_open())
    {
        int fsize = inFile.tellg();

        inFile.seekg(0, inFile.beg);
        if (!inFile.read((char *)memory, fsize))
            emitError("Cannot read from input file\n");

        while (true)
        {
            instWord = (unsigned char)memory[pc] |
                       (((unsigned char)memory[pc + 1]) << 8) |
                       (((unsigned char)memory[pc + 2]) << 16) |
                       (((unsigned char)memory[pc + 3]) << 24);

            // debugging: remove the line below when debugging is finished
            // cout << bitset<32>(instWord) << endl;

            pc += 4;
            // remove the following line once you have a complete simulator
            if (pc == 32)
                break; // stop when PC reached address 32
            instDecExec(instWord);
        }
    }
    else
        emitError("Cannot access input file\n");
}

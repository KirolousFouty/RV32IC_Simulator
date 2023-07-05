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
#include <cstdint>
// debugging: remove the line below when debugging is finished
#include <bitset>

using namespace std;

unsigned int pc;
unsigned char memory[(16 + 64) * 1024];
unsigned int reg[32];

void emitError(const char *s)
{
    cout << s;
    exit(0);
}

void printPrefix(unsigned int instA, unsigned int instW)
{
    cout << "0x" << hex << std::setfill('0') << std::setw(8) << instA << "\t0x" << std::setw(8) << instW;
}

void compInstDecExec(unsigned int instHalf)
{
}

void instDecExec(unsigned int instWord)
{
    unsigned int rd, rs1, rs2, funct3, funct7, opcode;
    unsigned int I_imm, S_imm, B_imm, U_imm, J_imm; // debugging: do we need SB_imm or UJ_imm ?
    unsigned int address;

    unsigned int instPC = pc - 4;

    opcode = instWord & 0x0000007F;
    rd = (instWord >> 7) & 0x0000001F;
    funct3 = (instWord >> 12) & 0x00000007;
    rs1 = (instWord >> 15) & 0x0000001F;
    rs2 = (instWord >> 20) & 0x0000001F;
    funct7 = (instWord >> 25) & 0x0000007F;

    // — inst[31] — inst[30:25] inst[24:21] inst[20]
    I_imm = ((instWord >> 20) & 0x7FF) | (((instWord >> 31) ? 0xFFFFF800 : 0x0)); // why the ORing?

    // Immediate value for U instructions
    U_imm = instWord;    // Storing whole instruction in immediate
    U_imm = U_imm >> 12; // Shifting right to exclude first 12 bits of instruction leaving us with the last 20 immediate bits
    U_imm = U_imm & 0x7FFFF;
    U_imm = U_imm << 12;
    // J_imm = U_imm;

    // Immediate value for B instructions
    B_imm = ((instWord >> 25) & 0x7F);
    B_imm = B_imm << 5;
    B_imm = B_imm + ((instWord >> 7) & 0x1F);
    S_imm = B_imm;

    // Immediate value for U instructions

    printPrefix(instPC, instWord);

    if (opcode == 0x33)
    { // R Instructions

        switch (funct3)
        {
        case 0x0:
        {
            if (funct7 == 0x0)
            {
                cout << "\tADD\tx" << dec << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                reg[rd] = reg[rs1] + reg[rs2];
            }
            else if (funct7 == 0x20)
            {
                cout << "\tSUB\tx" << dec << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                reg[rd] = reg[rs1] - reg[rs2];
            }
            break;
        }

        case 0x4:
        {
            if (funct7 == 0x00)
            {
                cout << "\tXOR\tx" << dec << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                reg[rd] = reg[rs1] ^ reg[rs2];
            }
            break;
        }

        case 0x6:
        {
            if (funct7 == 0x00)
            {
                cout << "\tOR\tx" << dec << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                reg[rd] = reg[rs1] | reg[rs2];
            }
            break;
        }

        case 0x7:
        {
            if (funct7 == 0x00)
            {
                cout << "\tAND\tx" << dec << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                reg[rd] = reg[rs1] & reg[rs2];
            }
            break;
        }

        case 0x1:
        {
            if (funct7 == 0x00)
            {
                cout << "\tSLL\tx" << dec << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                reg[rd] = rs1 << rs2; // debugging: should rs2 be cast to signed?
                // debugging: in RARS, if rs2 is negative, rs1 is set to 0
            }
            break;
        }

        case 0x5:
        {
            if (funct7 == 0x00)
            {
                cout << "\tSRL\tx" << dec << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                reg[rd] = rs1 >> rs2; // debugging: should rs2 be cast to signed?
                // debugging: in RARS, if rs2 is negative, rs1 is set to 0
            }
            else if (funct7 == 0x20)
            {
                cout << "\tSRA\tx" << dec << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                unsigned int temp = rs2;

                bool isNeg = reg[rs1] & 0x80000000;
                reg[rd] = rs1 >> rs2; // debugging: should rs2 be cast to signed?
                // debugging: in RARS, if rs1 is positive and rs2 is negative, rd is set to 0
                // debugging: in RARS, if rs1 is negative and rs2 is negative, rd is set to -1
                if (isNeg)
                {
                    for (unsigned int i = 0; i < temp; i++)
                    {
                        reg[rd] = reg[rd] | (isNeg << 31 - i);
                    }
                }
            }
            break;
        }

        case 0x2:
        {
            if (funct7 == 0x0)
            {
                cout << "\tSLT\tx" << dec << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                // debugging: should rs1 or rs2 be cast to signed?
                if (reg[rs1] < reg[rs2])
                    reg[rd] = 1;

                else
                    reg[rd] = 0;
            }
            break;
        }

        case 0x3:
        {
            if (funct7 == 0x0)
            {
                cout << "\tSLTU\tx" << dec << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                if (reg[rs1] < reg[rs2])
                    reg[rd] = 1;
                else
                    reg[rd] = 0;
            }
            break;
        }

        default:
            cout << "\tUnkown R Instruction \n";
        }
    }
    else if (opcode == 0x3B)
    {
        // R instructions
        cout << "\tUnkown R Instruction \n";
        // debugging: implementation required
        // debugging: check if opcode == 0x3B is unsupported (addw, subw, others)
    }
    else if (opcode == 0x13)
    {
        // I instructions
        switch (funct3)
        {
        case 0:
        {
            cout << "\tADDI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
            reg[rd] = reg[rs1] + (int)I_imm;
            // debugging: check the type cast (int)I_imm, and check for negative immediates
            break;
        }
        case 0x1:
        {
            cout << "\tSLLI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
            reg[rd] = rs1 << (int)I_imm;

            break;
        }
        case 0x2:
        {
            cout << "\tSLTI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
            if (reg[rs1] < (int)I_imm)
                reg[rd] = 0b1;
            else
                reg[rd] = 0b0;

            break;
        }
        case 0x3:
        {
            cout << "\tSLTIU\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
            if (reg[rs1] < (int)I_imm)
                reg[rd] = 0b1;
            else
                reg[rd] = 0b0;

            break;
        }

        case 0x4:
        {
            cout << "\tXORI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
            reg[rd] = reg[rs1] ^ (int)I_imm;
            break;
        }
        case 0x5:
        {
            if (funct7 == 0x00)
            {
                cout << "\tSRLI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
                reg[rd] = rs1 >> (int)I_imm;
            }
            else if (funct7 == 0x20)
            {
                cout << "\tSRAI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
                if ((int)I_imm < 0 && reg[rs1] >= 0)
                {
                    reg[rd] = 0b0;
                }
                else if ((int)I_imm < 0 && reg[rs1] < 0)
                {
                    reg[rd] = -1;
                }
                else
                {
                    unsigned int temp = rs2;

                    bool isNeg = reg[rs1] & 0x80000000;
                    reg[rd] = rs1 >> (int)I_imm;

                    for (unsigned int i = 0; i < temp; i++)
                    {
                        reg[rd] = reg[rd] | (isNeg << 31 - i);
                    }
                }
            }
            break;
        }

        case 0x6:
        {
            cout << "\tORI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
            reg[rd] = reg[rs1] | (int)I_imm;
            break;
        }

        case 0x7:
        {
            cout << "\tANDI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
            reg[rd] = reg[rs1] & (int)I_imm;
            break;
        }

        default:
            cout << "\tUnkown I Instruction \n";
        }
    }
    else if (opcode == 0x03)
    {
        int32_t sign_extended_value;
        signed int temp1;
        signed int temp2;
        unsigned int temp3;
        switch (funct3)
        {
        case 0x0:

        {
            cout << "\tlb\tx" << rd << ", " << dec << (int)I_imm << "(x" << rs1 << ")\n";
            temp1 = memory[reg[rs1] + I_imm];

            // sign extension
            if (temp1 & 0x00000080)
                sign_extended_value = temp1 | 0xFFFFFF00;

            else
                sign_extended_value = temp1 & 0x000000FF;

            reg[rd] = sign_extended_value;
            break;
        }

        case 0x1:

        {
            cout << "\tlh\tx" << rd << ", " << dec << (int)I_imm << "(x" << rs1 << ")\n";
            temp2 = memory[reg[rs1] + I_imm];

            // sign extension
            if (temp2 & 0x00008000)
                sign_extended_value = temp2 | 0xFFFF0000;

            else
                sign_extended_value = temp2 & 0x0000FFFF;

            reg[rd] = sign_extended_value;
            break;
        }
        case 0x2:
        {
            cout << "\tlw\tx" << rd << ", " << dec << (int)I_imm << "(x" << rs1 << ")\n";
            reg[rd] = memory[reg[rs1] + I_imm];
            break;
        }
        case 0x4:

        {
            cout << "\tlbu\tx" << rd << ", " << dec << (int)I_imm << "(x" << rs1 << ")\n";
            temp3 = memory[reg[rs1] + I_imm];
            int unsigned_extended_value = temp3 & 0x000000FF;
            reg[rd] = unsigned_extended_value;
            break;
        }
        case 0x5:
        {
            cout << "\tlhu\tx" << rd << ", " << dec << (int)I_imm << "(x" << rs1 << ")\n";
            unsigned int temp4 = memory[reg[rs1] + I_imm];
            int unsigned_extended_value = temp4 & 0x0000FFFF;
            reg[rd] = unsigned_extended_value;
            break;
        }
        }
    }
    else if (opcode == 0x73)
    {
        cout << "\tECALL\n";
        if (reg[17] == 1) // if a7==1 print a0 integer
        {
            cout << (int)reg[10] << endl;
        }
        else if (reg[17] == 4)
        { // if a7==4 print a0 string
            cout << memory[reg[10]] << endl;
        }
        else if (reg[17] == 10)
        {
            exit(0);
        }
    }
    else if (opcode == 0x0F)
    {
        // I instructions
        cout << "\tUnkown I Instruction \n";
        // debugging: implementation required
        // debugging: check if opcode == 0x0F is unsupported (fence, or other)
    }
    else if (opcode == 0x1B)
    {
        // I instructions
        cout << "\tUnkown I Instruction \n";
        // debugging: implementation required
        // debugging: check if opcode == 0x1B is unsupported (addiw, slliw, or others)
    }
    else if (opcode == 0x23)
    {
        S_imm = 0;
        // S instructions
        switch (funct3)
        {
        case 0x0:
        {
            cout << "\tSB\tx" << dec << rs1 << ", " << (int)S_imm << "(x" << rs2 << ")\n";

            unsigned int temp3 = memory[reg[rs1] + S_imm];
            reg[rs2] = temp3 & 0x000000FF;

            break;
        }

        case 0x1:
        {
            cout << "\tSH\tx" << dec << rs1 << ", " << (int)S_imm << "(x" << rs2 << ")\n";

            unsigned int temp4 = memory[reg[rs1] + S_imm];
            reg[rs2] = temp4 & 0x0000FFFF;

            break;
        }

        case 0x2:

        {
            cout << "\tSW\tx" << dec << rs1 << ", " << (int)S_imm << "(x" << rs2 << ")\n";

            reg[rs2] = memory[reg[rs1] + S_imm];

            break;
        }

        default:
            cout << "\tUnknown S Instruction\n";
        }
    }
    else if (opcode == 0x63)
    {
        // SB instructions
        switch (funct3)
        {
        case 0x0:
        {
            cout << "\tBEQ\tx" << dec << rs1 << ", x" << rs2 << ", " << hex << "0x" << B_imm << "\n";
            if (reg[rs1] == reg[rs2])
                pc += B_imm;
            break;
        }

        case 0x1:
        {
            cout << "\tBNE\tx" << dec << rs1 << ", x" << rs2 << ", " << hex << "0x" << B_imm << "\n";
            if (reg[rs1] != reg[rs2])
                pc += B_imm;
            break;
        }
        case 0x4:
        {
            cout << "\tBLT\tx" << dec << (int)rs1 << ", x" << (int)rs2 << ", " << hex << "0x" << B_imm << "\n";
            if (reg[rs1] < reg[rs2])
                pc += B_imm;
            break;
        }
        case 0x5:
        {
            cout << "\tBGE\tx" << dec << (int)rs1 << ", x" << (int)rs2 << ", " << hex << "0x" << B_imm << "\n";
            if (reg[rs1] > reg[rs2])
                pc += B_imm;
            break;
        }
        case 0x6:
        {
            cout << "\tBLTU\tx" << dec << rs1 << ", x" << rs2 << ", " << hex << "0x" << B_imm << "\n";
            if (reg[rs1] <= reg[rs2])
                pc += B_imm;
            break;
        }
        case 0x7:
        {
            cout << "\tBGEU\tx" << dec << rs1 << ", x" << rs2 << ", " << hex << "0x" << B_imm << "\n";
            if (reg[rs1] >= reg[rs2])
                pc += B_imm;
            break;
        }
        default:
            cout << "\tUnknown B Instruction\n";
        }
    }
    else if (opcode == 0x37) // LUI
    {
        reg[rd] = reg[rd] + (int)U_imm << 12;
        cout << "\tLUI\tx" << rd << ", " << hex << "0x" << (int)U_imm << "\n";
    }
    else if (opcode == 0x17) // AUIPC
    {
        reg[rd] = (int)pc + (int)U_imm;
        cout << "\tAUIPC\tx" << rd << ", " << hex << "0x" << (int)U_imm << "\n";
    }
    else if (opcode == 0x6F) // JAL
    {
        // debugging: check addition to pc
        reg[rd] = (int)pc + 4;
        pc += J_imm;
        cout << "\tJAL\tx" << rd << ", " << hex << "0x" << (int)J_imm << "\n";
        pc = reg[rd]; // debugging: remove before submission
    }
    else if (opcode == 0x67) // JALR
    {
        switch (funct3)
        {
        case 0x0:
            reg[rd] = (int)pc + 4;
            pc = reg[rs1] + (int)I_imm;
            cout << "\tJALR\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
            pc = reg[rd];
            break;
        default:
            cout << "\tUnkown I Instruction \n";
        }
    }
    else
    {
        cout << "\tUnkown Instruction Type \n";
    }
}

int main(int argc, char *argv[])
{
    int counter = 0;
    unsigned int instWord = 0;
    unsigned int instHalf = 0;
    int caseComp = 0;
    int caseNComp = 0;
    ifstream inFile;
    ofstream outFile;

    /*
        if (argc < 1)
            emitError("use: rvcdiss <machine_code_file_name>\n");

        inFile.open(argv[1], ios::in | ios::binary | ios::ate);
    */

    // debugging: remove the line below and uncomment the block above when debugging is finished
    inFile.open("t1.bin", ios::in | ios::binary | ios::ate);

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
            instHalf = (unsigned char)memory[pc] |
                       (((unsigned char)memory[pc + 1]) << 8);
            instWord = (unsigned char)memory[pc] |
                       (((unsigned char)memory[pc + 1]) << 8) |
                       (((unsigned char)memory[pc + 2]) << 16) |
                       (((unsigned char)memory[pc + 3]) << 24);
            counter++;

            // debugging: remove the line below when debugging is finished
            // cout << bitset<32>(instWord) << endl;

            // pc += 4;

            if (instWord == 0) // debugging: configure the best way to detect the end of the program, and the while(true) loop
                break;

            caseComp = instHalf & 3;
            caseNComp = instWord & 28;
            if (caseComp != 3)
            {
                if (caseNComp != 28)
                {
                    pc += 4;
                    instDecExec(instWord);
                }
                else
                {
                    pc += 2;
                    compInstDecExec(instHalf);
                }
            }
            else
            {
                if (caseNComp != 28)
                {
                    pc += 4;
                    instDecExec(instWord);
                }
            }
        }
    }
    else
        emitError("Cannot access input file\n");
}
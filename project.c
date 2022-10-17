#include "spimcore.h"

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    int op = ALUControl;
    
    if(op == 0) {
        *ALUresult = A + B;
    }
    else if(op == 1) {
        *ALUresult = A - B;
    }
    else if(op == 2) {
        if((signed) A < (signed) B)
            *ALUresult = 1;
        else
            *ALUresult = 0;
    }
    else if(op == 3) {
        if(A < B)
            *ALUresult = 1;
        else
            *ALUresult = 0;
    }
    else if(op == 4) {
        *ALUresult = A & B;
    }
    else if(op == 5) {
        *ALUresult = A | B;
    }
    else if(op == 6) {
        *ALUresult = B << 16;
    }
    else if(op == 7) {
        *ALUresult = ~A;
    }
    
    if(*ALUresult == 0) {
        *Zero = 1;
    }
    else
        *Zero = 0;
    
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction) {
    if(PC % 4 != 0) {
        return 1;
    }
    
    *instruction = Mem[PC >> 2];
    return 0;
        
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec) {
    *op = (instruction >> 26) & 0b00000000000000000000000000111111;
    *r1 = (instruction >> 21) & 0b11111;
    *r2 = (instruction >> 16) & 0b11111;
    *r3 = (instruction >> 11) & 0b11111;
    *funct = instruction & 0b00000000000000000000000000111111;
    *offset = instruction & 0b00000000000000001111111111111111;
    *jsec = instruction & 0b00000011111111111111111111111111;
}

/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
    // zero control signals
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 0;
        controls->MemWrite = 0;
        controls->ALUSrc = 0;
        controls->RegWrite = 0;

        if (op == 0) {
            controls->RegDst = 1;
            controls->ALUOp = 7;
            controls->RegWrite = 1;
            return 0;
        }
        
        else if (op == 8) {
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            return 0;
        }

        else if (op == 35) {
            controls->MemRead = 1;
            controls->MemtoReg = 1;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            return 0;
        }
        
        else if (op == 15) {
            controls->ALUSrc = 1;
            controls->ALUOp = 6;
            controls->RegWrite = 1;
            return 0;
        }
        
        else if (op == 43) {
            controls->MemWrite = 1;
            controls->ALUSrc = 1;
            return 0;
        }
        
        else if (op == 4) {
            controls->RegDst = 2;
            controls->Branch = 1;
            controls->MemtoReg = 2;
            controls->ALUOp = 1;
            return 0;
        }
        
        else if (op == 10) {
            controls->ALUOp = 2;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            return 0;
        }
        
        else if (op == 11) {
            controls->ALUOp = 3;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            return 0;
        }
        
        else if (op == 2) {
            controls->RegDst = 2;
            controls->Jump = 1;
            controls->Branch = 2;
            controls->MemtoReg = 2;
            controls->ALUSrc = 2;
            return 0;
        }
        
        return 1;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2) {
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
    if ((offset >> 15) == 1)
        *extended_value = offset | 0xFFFF0000;
    else
        *extended_value = offset & 0x0000FFFF;

}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
        if (ALUSrc == 1)
            data2 = extended_value;

        if (ALUOp == 0 || ALUOp == 1 || ALUOp == 2 || ALUOp == 3 || ALUOp == 4 || ALUOp == 5 || ALUOp == 6) {
            ALU(data1, data2, ALUOp, ALUresult, Zero);
            return 0;
        }

        if (ALUOp == 7 && ALUSrc == 0) {

            if (funct == 4)
                ALUOp = 6;

            else if (funct == 32)
                ALUOp = 0;
       
            else if (funct == 34)
                ALUOp = 1;
        
            else if (funct ==  36)
                ALUOp = 4;
        
            else if (funct == 37)
                ALUOp = 5;

            else if (funct == 39)
                ALUOp = 7;
        
            else if (funct == 42)
                ALUOp = 2;
        
            else if (funct == 43)
                ALUOp = 3;
        
            else
                return 1;

        ALU(data1, data2, ALUOp, ALUresult, Zero);

        return 0;
      }

    return 1;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem) {
    if(MemWrite == 1) {
        if((ALUresult % 4) == 0)
            Mem[ALUresult >> 2] = data2;
        else
            return 1;
    }
    if(MemRead == 1){
        if((ALUresult % 4) == 0)
            *memdata = Mem[ALUresult >> 2];
        else
            return 1;
    }
    
    return 0;
    
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg) {

    if (MemtoReg == 1 && RegDst == 0 && RegWrite == 1) {
        Reg[r2] = memdata;
    }
    else if (MemtoReg == 0 && RegDst == 1 && RegWrite == 1) {
        Reg[r3] = ALUresult;
    }
    else if (MemtoReg == 1 && RegDst == 1 && RegWrite == 1) {
        Reg[r3] = memdata;
    }
    else if (MemtoReg == 0 && RegDst == 0 && RegWrite == 1) {
        Reg[r2] = ALUresult;
    }
    
}


/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC) {

    *PC += 4;
    if(Jump == 1)
        *PC = (jsec << 2) | (*PC & 0xf0000000);
    if(Zero == 1 && Branch == 1)
        *PC += extended_value << 2;
    
}

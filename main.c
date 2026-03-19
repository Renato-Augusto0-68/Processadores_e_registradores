#include <stdio.h>
#include <stdint.h>

uint8_t mem[256] = {0};
uint8_t reg[4] = {0};
uint8_t pc = 0, zf = 0, running = 1;
int ciclo = 0;
int chave =0x3;

void fetch(uint8_t *op, uint8_t *a, uint8_t *b) {
    *op = mem[pc]; *a = mem[pc+1]; *b = mem[pc+2];
    pc += 3;
}

void decode_execute(uint8_t op, uint8_t a, uint8_t b) {
    switch (op) {
        case 0x01: reg[a] = mem[b]; break;
        case 0x02: mem[b] = reg[a]; break;
        case 0x03: reg[a] = reg[a] + chave; break;
        case 0x04: reg[a] = reg[a] - reg[b]; break;
        case 0x05: reg[a] = b; break;
        case 0x06: zf = (reg[a] == reg[b]) ? 1 : 0; break;
        case 0x07: pc = a; break;
        case 0x08: if (zf) pc = a; break;
        case 0x09: if (!zf) pc = a; break;
        case 0x0A: running = 0; break;  
    }
}
void func_set(){
    mem[0x10]=0x41; mem[0x11]=0x03; mem[0x12]=0x08;
    mem[0x13] ; mem[0x14]; mem[0x15]; 
    mem[0x16]; mem[0x17]; 

}


void trace(uint8_t op, uint8_t a, uint8_t b) {
    const char *nomes[] = {"","LOAD","STORE","ADD",
    "SUB","MOV","CMP","JMP","JZ","JNZ","HALT"};
    printf("Ciclo %d: %-5s %d,%d | R0=%3d R1=%3d"
    " R2=%3d R3=%3d | PC=%c ZF=%d\n",
    ciclo, nomes[op], a, b,
    reg[0], reg[1], reg[2], reg[3], pc, zf);
}


int main() {
// TODO: carregar programa e dados na mem[]
    while (running && pc < 256) {
        uint8_t op, a, b;
        ciclo++;
        
        fetch(&op, &a, &b);
        
        decode_execute(op, a, b);
        
        trace(op, a, b);
    }
    return 0;
}
#include <stdio.h>
#include <stdint.h>
#include <string.h>

uint8_t mem[256] = {0};
uint8_t reg[4] = {0};
uint8_t pc = 0x30, zf = 0, running = 1; 
int ciclo = 0;

void fetch(uint8_t *op, uint8_t *a, uint8_t *b) {
    *op = mem[pc]; 
    *a = mem[pc+1]; 
    *b = mem[pc+2];
    pc += 3;
}

void decode_execute(uint8_t op, uint8_t a, uint8_t b) {
    switch (op) {
        case 0x01: reg[a] = mem[b]; break;
        case 0x02: mem[b] = reg[a]; break;
        case 0x03: reg[a] = reg[a] + reg[b]; break; 
        case 0x04: reg[a] = reg[a] - reg[b]; break;
        case 0x05: reg[a] = b; break;
        case 0x06: zf = (reg[a] == reg[b]) ? 1 : 0; break;
        case 0x07: pc = a; break;
        case 0x08: if (zf) pc = a; break;
        case 0x09: if (!zf) pc = a; break;
        case 0x0A: running = 0; break;
        case 0x0B: reg[a] = mem[reg[b]]; break; 
        case 0x0C: mem[reg[b]] = reg[a]; break; 
        case 0x0D: reg[a]++; break;             
    }
}

void carregar_memoria(int escolha,int input_size) {
    mem[0x08] = 3; 
    if (escolha !=1){
        for(int i = 0; i < 8; i++) {
            mem[0x10 + i] = 65 + i;
        }
    }
    uint8_t programa[] = {
        0x05, 0x01, 0x10, // 0x30: MOV R1, 0x10  (R1 = Ponteiro de Origem)
        0x05, 0x02, 0x20, // 0x33: MOV R2, 0x20  (R2 = Ponteiro de Destino)
        0x01, 0x03, 0x08, // 0x36: LOAD R3, 0x08 (R3 = Chave '3')
        0x05, 0x00, 0x18, // 0x39: MOV R0, 0x18  (R0 = Limite do loop)
        0x02, 0x00, 0x09, // 0x3C: STORE R0, 0x09(Salva o limite na RAM temp)
        0x01, 0x00, 0x09, // 0x3F: LOAD R0, 0x09 (Recupera limite para R0)
        0x06, 0x01, 0x00, // 0x42: CMP R1, R0    (Ponteiro Origem == Limite?)
        0x08, 0x5A, 0x00, // 0x45: JZ 0x5A       (Se igual, pula para o HALT)
        0x0B, 0x00, 0x01, // 0x48: L_IND R0, R1  (Lê byte da Origem para R0)
        0x03, 0x00, 0x03, // 0x4B: ADD R0, R3    (Soma a Chave ao byte)
        0x0C, 0x00, 0x02, // 0x4E: S_IND R0, R2  (Salva byte no Destino)
        0x0D, 0x01, 0x00, // 0x51: INC R1        (Avança ponteiro de Origem)
        0x0D, 0x02, 0x00, // 0x54: INC R2        (Avança ponteiro de Destino)
        0x07, 0x3F, 0x00, // 0x57: JMP 0x3F      (Volta para o Início do Loop)
        0x0A, 0x00, 0x00  // 0x5A: HALT          (Desliga a CPU)
    };

    if (escolha!=1){
        for(int i = 0; i < sizeof(programa); i++) {
            mem[0x30 + i] = programa[i];
        }
    }
    else{
        programa[11] = (uint8_t)(0x10 + input_size);
        for(int i = 0; i < sizeof(programa); i++) {
            mem[0x30 + i] = programa[i];
        }
    }
}
int guardar_texto_byte(char *texto){
    int i=0;
    while (*texto!='\0'){
        if (*texto>=65 && *texto<=90 || *texto >=97 && *texto<=122){
            mem[0x10+i] = (uint8_t)(*(texto));
            i++;}
        texto++;
    }
    return i;
}

void trace(uint8_t op, uint8_t a, uint8_t b) {
    const char *nomes[] = {
        "???", "LOAD", "STORE", "ADD", "SUB", "MOV", "CMP", 
        "JMP", "JZ", "JNZ", "HALT", "L_IND", "S_IND", "INC"
    };
    
    // Proteção para identificar instruções desconhecidas
    const char *nome_op = (op <= 0x0D) ? nomes[op] : "INV";
    printf("Ciclo %2d: %-5s %02X,%02X | R0=%3d R1=%3d R2=%3d R3=%3d | PC=0x%02X ZF=%d\n",
           ciclo, nome_op, a, b,
           reg[0], reg[1], reg[2], reg[3], pc, zf);
}


void imprimir_resultado(int escolha) {
    if (escolha !=1){
        printf("\n=== RESULTADO NA MEMÓRIA ===\n");
        printf("Origem  (0x10-0x17): ");
        for(int i=0x10; i<=0x17; i++) printf("%d (%c) ", mem[i],mem[i]);
    
        printf("\nDestino (0x20-0x27): ");
        for(int i=0x20; i<=0x27; i++) printf("%d (%c) ", mem[i],mem[i]);
        printf("\n");}
    else{
        printf("\n=== RESULTADO NA MEMÓRIA ===\n");
        printf("String original (Origem) (0x10-0x17): ");
        for(int i=0x10; i<=0x17 && mem[i]!='\0'; i++) printf("%c (%d) ", mem[i],mem[i]);
    
        printf("\nString alterada (Destino) (0x20-0x27): ");
        for(int i=0x20; i<=0x27 && mem[i]!='\0'; i++) printf("%c (%d) ", mem[i],mem[i]);
        printf("\n");
    }
}

int main() {
    int decisao=0;
    printf("Você deseja digitar uma string? 1 se sim, 0 se não. ");
    scanf("%d",&decisao);
    if (decisao ==1){
        while (getchar()!='\n'){};
        char texto[8];
        printf("Digite a string, no máximo 8 caracteres, para ser convertida: ");
        fgets(texto,sizeof(texto),stdin);
        texto[strcspn(texto, "\n")] =' ';
        int input_size =guardar_texto_byte(texto);
        carregar_memoria(decisao,input_size );
        while(running && pc<256){
           uint8_t op, a, b;
           ciclo++;
           fetch(&op, &a, &b);
           decode_execute(op, a, b);
           trace(op, a, b); 
        }
        imprimir_resultado(decisao);
    }else{
        carregar_memoria(decisao,decisao);
        while (running && pc < 256) {
            uint8_t op, a, b;
            ciclo++;
            fetch(&op, &a, &b);
            decode_execute(op, a, b);
            trace(op, a, b);
        }
        imprimir_resultado(decisao);
    }
      return 0;
}

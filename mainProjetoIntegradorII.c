
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura para os registradores. (usando int32 para garantir que o tamanho seja de 32 bits ou 4 bytes)
typedef struct {
    int registradores[32]; // Registradores  ($0 - $31)
    int HI;  // Registrador HI
    int LO;      // Registrador LO
    unsigned int PC;                // Contador de programa (Program Counter)
} RegistradoresMIPS;

typedef struct Memoria vetorMemoria;
struct Memoria{
	char palavra[16];
	int opcode;
	int rs;
	int rt;
	int rd;
	int funct;
	int imm;
	int addr;
};

void inicializar_registradores(RegistradoresMIPS *mips); 
void escrever_registrador(RegistradoresMIPS *mips, int reg, int valor);
int ler_registrador(RegistradoresMIPS *mips, int reg);
void imprimir_registradores(RegistradoresMIPS *mips);

int pot(int base, int expoente); // Potencia
int cb2d(char complemento[]); // Conversao binario(complemento de dois)/decimal
int cbd(char binario[]); // Conversao binario/decimal
void omi(vetorMemoria* memoria, char palavra[]); // OMI = Organiza Memnbros da Instrucao
void casm(char palavra[], char nomeArquivo[]); // CASM = Coverte p/ .asm
vetorMemoria carregaMemoria(char nomeArquivo[]); // Carega o arquivo externopara um vetor





int main(void)
{
	RegistradoresMIPS mips;
	
	vetorMemoria memoria[256];
	FILE* arquivo = NULL;
	char palavra[17];
	char nomeArquivo[256];
	char arquivoAsm[10] = "saida.asm";
	int contadorInstrucoes = 0, e l = 0;
	
	inicializar_registradores(&mips);
	
	do{
	
	printf("\n******************************* MENU *******************************\n");
	printf("|\t(0) Carregar memória de instruções (.mem)\n");
	printf("|\t(1) Carregar memória de Dados (.dat)\n");
	printf("|\t(2) Imprimir memórias (instruções e dados)\n");
	printf("|\t(3) Imprimir banco de registradores\n");
	printf("|\t(4) Imprimir todo o simulador (registradores e memórias)\n");
	printf("|\t(5) Salvar .asm\n");
	printf("|\t(6) Salvar .dat\n");
	printf("|\t(7) Executar o programa (Run)\n");
	printf("|\t(8) Executar o programa (Step)\n");
	printf("|\t(9) Voltar uma instrucao (Back)\n");
	printf("*********************************************************************\n\n");
	
	printf("Sua escolha:\t");
	scanf("%d", &e);
	printf("\n");
	
	switch (e){
		case 0:
			printf("Nome do arquivo:\t");
			setbuf(stdin, NULL);
			scanf("%[^\n]s",nomeArquivo);
			printf("\n");
			getchar();
			
			if((arquivo = fopen(nomeArquivo,"r")) == NULL ){
				printf("Falha ao abrir arquivo...\n\n");
			} else{
		
			while(!feof(arquivo)){
				fgets(palavra,(sizeof(palavra)),arquivo); // Recebe a palavra de instrucao
				printf("tamanho da palavra %d\n",(int)strlen(palavra));
			
				if(strlen(palavra) == 16){			
				printf("%s\n", palavra); //palavra ainda contém o \n final e não pode considerá-lo na conversão
				//enderecoMemoria = &memoria[contadorInstrucoes]; // enderecoMemoria recebe o endereco(ponteiro) de uma posicao do vetor
				//omi(enderecoMemoria, palavra); // OMI = organiza membros da instrucao || contadorInstrucoes = posicao do vetor
				omi(&memoria[contadorInstrucoes], palavra); //não precisa do ponteiro pois pode passar o endereço direto da variável (que é um ponteiro)
				casm(palavra, arquivoAsm);
				palavra[0]='\0'; //só para zerar o vetor de instrução
			
				printf("instrução:\n");
				printf("%d\n", memoria[contadorInstrucoes].opcode);
				printf("%d\n", memoria[contadorInstrucoes].rs);
				printf("%d\n", memoria[contadorInstrucoes].rt);
				printf("%d\n", memoria[contadorInstrucoes].rd);
				printf("%d\n", memoria[contadorInstrucoes].imm);
				printf("%d\n", memoria[contadorInstrucoes].addr);
				printf("%d\n", memoria[contadorInstrucoes].funct);
				contadorInstrucoes ++; // Conta para saber quantas instrucoes devem ser lidas na memoria posteriormente...
				}
			}
			}
			break;
		
		case 1: // Carregar .dat
			
			break;
			
		case 2: // Imprimir memorias (instrucoes e dados)
			
			break;
			
		case 3: // Imprimir banco de registradores
			imprimir_registradores(&mips);
			printf("\n");
			break;
			
		case 4: // Imprimir todo o simulador (registadores e memorias)
			
			break;
		
		case 5: // Salvar .asm
			
			break;
			
		case 6: // Salvar .dat
			
			break;
			
		case 7: // Executa programa (run)
			
			break;
		
		case 8: // Exeuta programa (step)
			
			
			break;
			
		case 9: // Volta uma instrucao (back)
			
			break;
	}
	} while(l != 1);	
			
/*	printf("%d\n", memoria[0].imm);
	printf("%d\n", memoria[2].imm);
	printf("%d\n", memoria[4].rs);
	printf("%d\n", contadorInstrucoes);
	getchar();
*/	
	
	
	
	return 0;
}


int pot(int base, int expoente){
	int valor = base, i;
	if(expoente > 0){
		for(i=1; i<expoente; i++)  // Caso um, retorna sem alteracoes
			valor = valor * base;
		
		return(valor);
	}
	return(1); // Caso zero, retorna 1
}

int cb2d(char complemento[]){ // CB2D = Conversao binaria(complemento de dois)/decimal
	int potencia = 0, decimal = 0;
	int indice;
	
	indice = strlen(complemento) -1; // indice recebe tamanho do vetor -1 para percorrer coretamente durante o laço 
	while(complemento[indice] != '1'){ // Busca a primeira posicao preenchida com o caractere '1'
		potencia++; // potencia sobe conforme o vetor é percorrido partindo do fim (bit menos significativo)
		indice--;   // indice que percorre o vetor
	}
	decimal = decimal + pot(2, potencia); // Incrementa a variavel de retorno uma vez para o primeiro caractere '1'
	potencia++;
	indice--;
	while(indice >= 0){ // Continua percorrendo o vetor, desa vez ate o final...
		if(complemento[indice] == '0'){
			decimal = decimal + pot(2, potencia); // Incrementa o vetor agora quando o caractere igual a '0'
		}
		potencia++;
		indice--;
	}
	decimal = decimal *(-1); // Retorna o valor resultante negativo(multipicado por -1)
	return (decimal);
}

int cbd(char binario[]){ // CBD = conversao binaria/decimal
	int potencia = 0, decimal = 0;
	int indice;
	
	indice = strlen(binario) -1; // indice recebe tamanho do vetor -1 para percorrer coretamente durante o laço 
	while(indice >= 0){
		if(binario[indice] == '1'){ // apenas há o incremento na variavel de retorno caso o caractere seja "1" em tal indice
			decimal = decimal + pot(2, potencia);
		}
		potencia++; // potencia sobe conforme o vetor é percorrido partindo do fim (bit menos significativo)
		indice--;   // indice que percorre o vetor
	}
	return (decimal);
}

void omi(vetorMemoria* memoria, char palavra[]){ // OMI = Organiza membros da instrucao
	
	char binarioVI[6], binarioIV[4], binarioIII[3];
	int indiceS; // Indice Switch
	int decimal; // Variavel para preencher o vetor memoria
	strncpy(binarioIV, palavra +0, 4);
	binarioIV[4] = '\0';
	indiceS = cbd(binarioIV); // Conversao do opcode para decimal(Inteiro), a fim de acessar o Switch
	printf("opcode reconhecido %d\n",indiceS);
	switch (indiceS){
		case 0:
			strcpy(memoria->palavra, palavra); // Guarda a palavra de instrucao
			
			memoria->opcode = indiceS; // indiceS Possui decimal do opcode
			
			strncpy(binarioIII, palavra +4, 3); // Copia para a string[3] as tres primeiras posicoes da palavra[] apartir da quarta posicao
			binarioIII[3] = '\0'; // Sinaliza o fim da String
			decimal = cbd(binarioIII); // Conversao binario(string)/decimal(int)
			memoria->rs = decimal; // Membro recebe valor decimal		RS
			
			strncpy(binarioIII, palavra +7, 3); // ||		...	Apartir da setima posicao
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			memoria->rt = decimal; // RT
			
			strncpy(binarioIII, palavra +10, 3);
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			memoria->rd = decimal; // RD
			
			strncpy(binarioIII, palavra +13, 3);
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			memoria->funct = decimal; // FUNCT
			break;
		
		case 2:
			strcpy(memoria->palavra, palavra); // PALAVRA
			break;
		
		case 4:
			strcpy(memoria->palavra, palavra); // PALAVRA
			
			memoria->opcode = indiceS; // OPCODE

			strncpy(binarioIII, palavra +4, 3); 
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			memoria->rs = decimal; // RS
			
			strncpy(binarioIII, palavra +7, 3);
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			memoria->rt = decimal; // RT
			
			strncpy(binarioVI, palavra +10, 6);
			binarioVI[6] = '\0';
			if(binarioVI[0] == '1')
				decimal = cb2d(binarioVI); // cb2d = Conversao binario(complemento de dois)/decimal
			else
				decimal = cbd(binarioVI);
			memoria->imm = decimal; // IMM
			break;
			
		case 8:
			strcpy(memoria->palavra, palavra); // PALAVRA
			
			memoria->opcode = indiceS; // OPCODE

			strncpy(binarioIII, palavra +4, 3); 
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			memoria->rs = decimal; // RS
			
			strncpy(binarioIII, palavra +7, 3);
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			memoria->rt = decimal; // RT
			
			strncpy(binarioVI, palavra +10, 6);
			binarioVI[6] = '\0';
			decimal = cbd(binarioVI);
			memoria->addr = decimal; // ADDR
			break;
			
		case 11:
			strcpy(memoria->palavra, palavra); // PALAVRA
			
			memoria->opcode = indiceS; // OPCODE
			
			strncpy(binarioIII, palavra +4, 3); 
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			memoria->rs = decimal; // RS
			
			strncpy(binarioIII, palavra +7, 3);
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			memoria->rt = decimal; // RT
			
			strncpy(binarioVI, palavra +10, 6);
			binarioVI[6] = '\0';
			if(binarioVI[0] == '1')
				decimal = cb2d(binarioVI); // cb2d = Conversao binario(complemento de dois)/decimal
			else
				decimal = cbd(binarioVI); // 											RT É O DESTINO!!
			memoria->addr = decimal; // ADDR
			break;
				
		case 15:
			strcpy(memoria->palavra, palavra); // PALAVRA
			
			memoria->opcode = indiceS; // OPCODE
			
			strncpy(binarioIII, palavra +4, 3); 
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			memoria->rs = decimal; // RS
			
			strncpy(binarioIII, palavra +7, 3);
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII); // 												ADDR É O DESTINO!!!!
			memoria->rt = decimal; // RT
			
			strncpy(binarioVI, palavra +10, 6);
			binarioVI[6] = '\0';
			if(binarioVI[0] == '1')
				decimal = cb2d(binarioVI); // cb2d = Conversao binario(complemento de dois)/decimal
			else
				decimal = cbd(binarioVI); // 											
			memoria->addr = decimal; // ADDR
			break;
	}
}


void casm(char palavra[], char nomeArquivo[]){			// casm = Converte para .asm
	char binarioVI[6], binarioIV[4], binarioIII[3];
	FILE* arquivo;
	int indiceS; // Indice Switch
	int funct; // Variavel para preencher o vetor memoria
	int rs, rt, rd, imm, addr; // r(*) = registradores da instrucao
	strncpy(binarioIV, palavra +0, 4);
	binarioIV[4] = '\0';
	indiceS = cbd(binarioIV); // Conversao do opcode para decimal(Inteiro), a fim de acessar o Switch
	printf("opcode reconhecido %d\n", indiceS);
	switch (indiceS){
		case 0: // Tipo R
			strncpy(binarioIII, palavra +13, 3);
			binarioIII[3] = '\0';
			funct = cbd(binarioIII);
			
			strncpy(binarioIII, palavra +4, 3); // Copia para a string[3] as tres primeiras posicoes da palavra[] apartir da quarta posicao
			binarioIII[3] = '\0'; // Sinaliza o fim da String
			rs = cbd(binarioIII); // Conversao binario(string)/decimal(int)
					
			strncpy(binarioIII, palavra +7, 3); // ||		...	Apartir da setima posicao
			binarioIII[3] = '\0';
			rt = cbd(binarioIII);
					
			strncpy(binarioIII, palavra +10, 3);
			binarioIII[3] = '\0';
			rd = cbd(binarioIII);
					
			switch (funct){ // Funct
				case 0:
					if((arquivo = fopen(nomeArquivo, "a")) == NULL)
						printf("Problema ao abrir o arquivo!\n");
					else{
						fprintf(arquivo, "ADD $r%d, $r%d, $r%d\n", rd, rs, rt);
						fclose(arquivo);
					}
									
					break;
					
				case 2:
					if((arquivo = fopen(nomeArquivo, "a")) == NULL)
						printf("Problema ao abrir o arquivo!\n");
					else{
						fprintf(arquivo, "SUB $r%d, $r%d, $r%d\n", rd, rs, rt);
						fclose(arquivo);
					}
											
					break;
					
				case 4:
					if((arquivo = fopen(nomeArquivo, "a")) == NULL)
						printf("Problema ao abrir o arquivo!\n");
					else{
						fprintf(arquivo, "AND $r%d, $r%d, $r%d\n", rd, rs, rt);
						fclose(arquivo);
					}
										
					break;
					
				case 5:
					
					if((arquivo = fopen(nomeArquivo, "a")) == NULL)
						printf("Problema ao abrir o arquivo!\n");
					else{
						fprintf(arquivo, "OR $r%d, $r%d, $r%d\n", rd, rs, rt);
						fclose(arquivo);
					}
					
					break;
			}
			break;
		
		/*case 2:
			strcpy(memoria->palavra, palavra); // PALAVRA
			break;*/
		
		case 4: // Tipo I (addi)
			strncpy(binarioIII, palavra +4, 3); 
			binarioIII[3] = '\0';
			rs = cbd(binarioIII);
			
			strncpy(binarioIII, palavra +7, 3);
			binarioIII[3] = '\0';
			rt = cbd(binarioIII);
			
			strncpy(binarioVI, palavra +10, 6);
			binarioVI[6] = '\0';
			if(binarioVI[0] == '1')
				imm = cb2d(binarioVI); // cb2d = Conversao binario(complemento de dois)/decimal
			else
				imm = cbd(binarioVI);
				
			if((arquivo = fopen(nomeArquivo, "a")) == NULL)
				printf("Falha ao abrir arquivo!\n");
			else{
				fprintf(arquivo, "ADDI $r%d, $r%d, %d\n", rt, rs, imm);
				fclose(arquivo);
			}
			
			break;
			
		case 8:
			strncpy(binarioIII, palavra +4, 3); 
			binarioIII[3] = '\0';
			rs = cbd(binarioIII);
			
			strncpy(binarioIII, palavra +7, 3);
			binarioIII[3] = '\0';
			rt = cbd(binarioIII);
			
			strncpy(binarioVI, palavra +10, 6);
			binarioVI[6] = '\0';
			addr = cbd(binarioVI);
			if((arquivo = fopen(nomeArquivo, "a")) == NULL)
				printf("Falha ao abrir arquivo!\n");
			else{
				fprintf(arquivo, "BEQ $r%d, $r%d, %d\n", rs, rt, addr);
				fclose(arquivo);
			}
			break;
			
		case 11:
			strncpy(binarioIII, palavra +4, 3); 
			binarioIII[3] = '\0';
			rs = cbd(binarioIII);
			
			strncpy(binarioIII, palavra +7, 3);
			binarioIII[3] = '\0';
			rt = cbd(binarioIII);
			
			strncpy(binarioVI, palavra +10, 6);
			binarioVI[6] = '\0';
			addr = cbd(binarioVI);
			
			if((arquivo = fopen(nomeArquivo, "a")) == NULL)
				printf("Falha ao abrir o arquivo!\n");
			else{
				fprintf(arquivo, "LW $r%d, %d($r%d)\n", rt, addr, rs);
				fclose(arquivo);
			}
			break;
				
		case 15:

			
			strncpy(binarioIII, palavra +4, 3); 
			binarioIII[3] = '\0';
			rs = cbd(binarioIII);
			
			strncpy(binarioIII, palavra +7, 3);
			binarioIII[3] = '\0';
			rt = cbd(binarioIII);
			
			strncpy(binarioVI, palavra +10, 6);
			binarioVI[6] = '\0';
			addr = cbd(binarioVI);
			
			if((arquivo = fopen(nomeArquivo, "a")) == NULL)
				printf("Falha ao abrir arquivo!\n");
			else{
				fprintf(arquivo, "SW $r%d, %d($r%d)\n", rt, addr, rs);
				fclose(arquivo);
			}
			break;
}	
}

// Inicializa os registradores (PC começa em 0, demais com valores padrão)
void inicializar_registradores(RegistradoresMIPS *mips) {
    for (int i = 0; i < 32; i++) {
        mips->registradores[i] = 0;
    }
    mips->HI = 0;
    mips->LO = 0;
    mips->PC = 0;
}

   // Escreve em um registrador, e garante que sempre seja zero no $0 
void escrever_registrador(RegistradoresMIPS *mips, int reg, int valor) {
    if (reg != 0 && reg >= 0 && reg < 32) {
        mips->registradores[reg] = valor;
    }
}


// Le o valor de algum registrador
int ler_registrador(RegistradoresMIPS *mips, int reg) {
    if (reg >= 0 || reg < 32) {
        return mips->registradores[reg];
    }
    return 0;
}



    // mostrar  os valores dos registradores para o nosso auxilio
void imprimir_registradores(RegistradoresMIPS *mips) {
    printf("PC: %u\n", mips->PC);
    printf("HI: %d, LO: %d\n", mips->HI, mips->LO);
    for (int i = 0; i < 32; i++) {
        printf("$%d: %d\t", i, mips->registradores[i]);
        if (i % 4 == 3) printf("\n");
    }
}

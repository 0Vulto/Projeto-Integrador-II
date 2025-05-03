#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MEM_SIZE 256 // Define o tamanho da memória como 256 bytes

typedef struct {
    int resul;
    int OF;
    int zero;
} ResulULA;

typedef struct{
	int pc;
	int rd;
	int valRd;
	int mem;
	int valMem;
	int tipo;
} back;

// Estrutura para os registradores. (usando int32 para garantir que o tamanho seja de 32 bits ou 4 bytes)
typedef struct {
    int registradores[8]; // Registradores  ($0 - $7)
} RegistradoresMIPS;

typedef struct {
	int dados[256];
} dadosMem;

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

void inicializa_memdados(dadosMem* memoria);
back controle(vetorMemoria* memoria, RegistradoresMIPS* mips,  unsigned int *PC, dadosMem* dados);
void inicializar_registradores(RegistradoresMIPS *mips); 
void escrever_registrador(RegistradoresMIPS *mips, int reg, int valor);
int ler_registrador(RegistradoresMIPS *mips, int reg);
void imprimir_registradores(RegistradoresMIPS *mips, unsigned int* PC);
ResulULA ula(int val1, int val2, int ulaOp);
int pot(int base, int expoente); // Potencia
int cb2d(char complemento[]); // Conversao binario(complemento de dois)/decimal
int cbd(char binario[]); // Conversao binario/decimal
void omi(vetorMemoria* memoria, char palavra[]); // OMI = Organiza Memnbros da Instrucao
void casm(vetorMemoria memoria); // CASM = Coverte p/ .asm
void inicializa_mem(vetorMemoria* memoria);
void imprime_mem(vetorMemoria* memoria);
void voltarInstrucao (back alteracoes, unsigned int* pc, RegistradoresMIPS* mips, dadosMem* memoriaDados);
void ImprimeDados(dadosMem* memoria);
void carregar_memoria(dadosMem* memoriaDados);
void salvar_memoria(char filename[], dadosMem memoria[]);

int main(void){
	int i;
	back alteracoes;
	char nomedat[30];
	RegistradoresMIPS mips;
	unsigned int PC=0;                // Contador de programa (PC)
	dadosMem memoriaDados;
	vetorMemoria memoria[MEM_SIZE];
	int contadorInstrucoes = 0, e;
	inicializar_registradores(&mips);
	inicializa_mem(memoria);
	inicializa_memdados(&memoriaDados);
	do{
		printf("\n******************************* MENU ************************************\n");
		printf("|\t(0) Carregar memória de instruções (.mem)\t\t\t|\n");
		printf("|\t(1) Carregar memória de Dados (.dat)\t\t\t\t|\n");
		printf("|\t(2) Imprimir memórias (instruções e dados)\t\t\t|\n");
		printf("|\t(3) Imprimir banco de registradores\t\t\t\t|\n");
		printf("|\t(4) Imprimir todo o simulador (registradores e memórias)\t|\n");
		printf("|\t(5) Salvar .asm\t\t\t\t\t\t\t|\n");
		printf("|\t(6) Salvar .dat\t\t\t\t\t\t\t|\n");
		printf("|\t(7) Executar o programa (Run)\t\t\t\t\t|\n");
		printf("|\t(8) Executar o programa (Step)\t\t\t\t\t|\n");
		printf("|\t(9) Voltar uma instrucao (Back)\t\t\t\t\t|\n");
		printf("|\t(10)-----------Sair-----------\t\t\t\t\t|\n");
		printf("*************************************************************************\n\n");
		printf("Sua escolha:\t");
		setbuf(stdin, NULL);
		scanf("%d", &e);
		printf("\n");
		switch (e){
			case 0: //carrega .mem
				carregaMem(&contadorInstrucoes, memoria);
				break;
			
			case 1: // Carregar .dat
				carregar_memoria(&memoriaDados);
				break;
				
			case 2: // Imprimir memorias (instrucoes e dados)
				imprime_mem(memoria);
				printf("\n");
				ImprimeDados(&memoriaDados);
				break;
				
			case 3: // Imprimir banco de registradores
				imprimir_registradores(&mips, &PC);
				break;
				
			case 4: // Imprimir todo o simulador (registadores e memorias)
				imprimir_registradores(&mips, &PC);
				printf("\n");
				imprime_mem(memoria);
				printf("\n");
				ImprimeDados(&memoriaDados);
				break;
			
			case 5: // Salvar .asm
				for(i=0; i<=contadorInstrucoes; i++)
					casm(memoria[i]);
				break;
				
			case 6: // Salvar .dat
				printf("Apresente o nome do arquivo saida\n(exemplo: mem.dat)");
				setbuf(stdin,NULL);
				scanf("%s", nomedat);
				salvar_memoria(nomedat, &memoriaDados);
				break;
				
			case 7: // Executa programa (run)
				do{
					alteracoes = controle(&memoria[PC], &mips, &PC, &memoriaDados);
				}while((memoria[PC].opcode)+(memoria[PC].rs)+(memoria[PC].rt)+(memoria[PC].rd)!=0);//(strcmp(memoria->palavra,"\000")!=0);
				printf("Todas as instrucoes foram realizadas!\n");
				break;
			
			case 8: // Exeuta programa (step)
				if((memoria[PC].opcode)+(memoria[PC].rs)+(memoria[PC].rt)+(memoria[PC].rd)!=0){
					alteracoes = controle(&memoria[PC], &mips, &PC, &memoriaDados);	
				}else{
					printf("Nao existem mais instrucoes!\n");
				}
				break;
				
			case 9: // Volta uma instrucao (back)
				voltarInstrucao(alteracoes, &PC, &mips, &memoriaDados);
				break;
				
			case 10: //exit
				printf("Encerrando programa...");
				break;
		}
	} while(e != 10);	
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
	
	char binarioVI[6], binarioIV[4], binarioIII[3], binarioII[12];
	int indiceS; // Indice Switch
	int decimal; // Variavel para preencher o vetor memoria
	strncpy(binarioIV, palavra +0, 4);
	binarioIV[4] = '\0';
	indiceS = cbd(binarioIV); // Conversao do opcode para decimal(Inteiro), a fim de acessar o Switch
	switch (indiceS){
		case 0://tipo R
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
		
		case 2:// tipo j
			strcpy(memoria->palavra, palavra); // PALAVRA
			memoria->opcode = indiceS;
			
			strncpy(binarioII, palavra +4, 12);
			binarioII[12] = '\0';
			memoria->addr = cbd(binarioII);
			break;
		//tipo i
		case 4: //ADDi
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
			
		case 8: //BEQ
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
			
		case 11: //LW
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
			decimal = cbd(binarioVI); // 											RT É O DESTINO!!
			memoria->addr = decimal; // ADDR
			break;
				
		case 15: //SW
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
			decimal = cbd(binarioVI); 										
			memoria->addr = decimal; // ADDR
			break;
	}
}


void casm(vetorMemoria memoria){			// casm = Converte para .asm
	FILE* arquivo;
	char nomeArquivo[10] = "saida.asm";
	if((arquivo = fopen(nomeArquivo, "a")) == NULL)
		printf("Problema ao abrir o arquivo!\n");
	else{
		int indiceS, funct; // Indice Switch
		indiceS = memoria.opcode; // Conversao do opcode para decimal(Inteiro), a fim de acessar o Switch
		funct = memoria.funct;
		printf("opcode reconhecido %d\n", indiceS);
		switch (indiceS){
			case 0: // Tipo R
				switch (funct){ // Funct
					case 0:
						if(memoria.rs != 0 || memoria.rt != 0 || memoria.rd != 0)
							fprintf(arquivo, "ADD $r%d, $r%d, $r%d\n", memoria.rd, memoria.rs, memoria.rt);
						break;
						
					case 2:
						fprintf(arquivo, "SUB $r%d, $r%d, $r%d\n", memoria.rd, memoria.rs, memoria.rt);
						break;
						
					case 4:
						fprintf(arquivo, "AND $r%d, $r%d, $r%d\n", memoria.rd, memoria.rs, memoria.rt);
						break;
						
					case 5:
						fprintf(arquivo, "OR $r%d, $r%d, $r%d\n", memoria.rd, memoria.rs, memoria.rt);
						break;
				}
				break;
			
			case 2:
				fprintf(arquivo, "J %d\n", memoria.addr);
				break;
			
			case 4:
				fprintf(arquivo, "ADDI $r%d, $r%d, %d\n", memoria.rt, memoria.rs, memoria.imm);
				break;
				
			case 8:
				fprintf(arquivo, "BEQ $r%d, $r%d, %d\n", memoria.rt, memoria.rs, memoria.addr);
				break;
				
			case 11:
				fprintf(arquivo, "LW $r%d, %d($r%d)\n", memoria.rt, memoria.addr, memoria.rs);
				break;
					
			case 15:
				fprintf(arquivo, "SW $r%d, %d($r%d)\n", memoria.rt, memoria.addr, memoria.rs);
				break;
			}	
		fclose(arquivo);
	}
}

// Inicializa os registradores (PC começa em 0, demais com valores padrão)
void inicializar_registradores(RegistradoresMIPS *mips) {
    for (int i = 0; i < 8; i++) {
        mips->registradores[i] = 0;
    }
}

void ImprimeDados(dadosMem* memoria){
	printf("\n---------------------------- Memoria de Dados ---------------------------\n");
	printf("Legenda: Posicao [Conteudo]\n");
	for(int  i=0; i<256; i++){
		printf("%d [%d]\t", i, memoria->dados[i]);
	}
}

   // Escreve em um registrador, e garante que sempre seja zero no $0 
void escrever_registrador(RegistradoresMIPS *mips, int reg, int valor) {
    if (reg != 0 && reg >= 0 && reg < 32) {
        mips->registradores[reg] = valor;
    }
}

// Le o valor de algum registrador
int ler_registrador(RegistradoresMIPS *mips, int reg) {
    if (reg >= 0 && reg < 8) {
        return mips->registradores[reg];
    }
    return 0;
}

    // mostrar  os valores dos registradores para o nosso auxilio
void imprimir_registradores(RegistradoresMIPS *mips, unsigned int *PC) {
    printf("PC: %d\n", *PC);
    for (int i = 0; i < 8; i++) {
        printf("$%d: %d\t", i, mips->registradores[i]);
        if (i % 4 == 3) printf("\n");
    }
}

back controle(vetorMemoria* memoria, RegistradoresMIPS* mips, unsigned int* PC, dadosMem* memDados) {
    back alteracoes;
    ResulULA val;
    int val_rs = ler_registrador(mips, memoria->rs);//Real valor das variaveis
    int val_rt = ler_registrador(mips, memoria->rt);
    alteracoes.pc = (*PC);
    (*PC)++;
    alteracoes.rd = 128;
    alteracoes.valRd = 128;
    
	switch (memoria->opcode) {
		case 0: // Tipo R
			alteracoes.tipo = 0;
			switch (memoria->funct) {
				case 0: // ADD
					alteracoes.rd = memoria->rd;
					alteracoes.valRd = ler_registrador(mips, memoria->rd);
					val = ula(val_rs, val_rt, 0);
					escrever_registrador(mips, memoria->rd, val.resul);
					break;
				case 2: // SUB
					alteracoes.rd = memoria->rd;
					alteracoes.valRd = ler_registrador(mips, memoria->rd);
					val =  ula(val_rs, val_rt, 2);
					escrever_registrador(mips, memoria->rd, val.resul);
					break;
				case 4: // AND
					alteracoes.rd = memoria->rd;
					alteracoes.valRd = ler_registrador(mips, memoria->rd);
					val = ula(val_rs, val_rt, 4);
					escrever_registrador(mips, memoria->rd, val.resul);
					break;
				case 5: // OR
					alteracoes.rd = memoria->rd;
					alteracoes.valRd = ler_registrador(mips, memoria->rd);
					val =ula(val_rs, val_rt, 5);
					escrever_registrador(mips, memoria->rd, val.resul);
					break;
			}
			break;

		case 4: // ADDi
			alteracoes.tipo = 0;
			alteracoes.rd = memoria->rt;
			alteracoes.valRd = ler_registrador(mips, memoria->rt);
			val = ula(val_rs, memoria->imm, 0);
			escrever_registrador(mips, memoria->rt, val.resul);
			break;

		case 11: // LW
			alteracoes.tipo = 0;
			alteracoes.rd = memoria->rt;
			alteracoes.valRd = ler_registrador(mips, memoria->rt);
			val = ula(val_rs, memoria->imm, 0); 
			val.resul = memDados->dados[val.resul]; 
			escrever_registrador(mips, memoria->rt, val.resul);
			break;

		case 15: // SW
			alteracoes.tipo = 1;
			alteracoes.mem = memoria->addr;
			alteracoes.valMem = memDados->dados[memoria->addr];
			val = ula(val_rs, memoria->addr, 0); 
			memDados->dados[val.resul] = val_rt; 
			break;

		case 8: // BEQ
			alteracoes.tipo = -1;
			val = ula(val_rs, val_rt, 2);
			if(val.zero == 0)
				(*PC) = (*PC)+memoria->addr;
			break;

		case 2: // JUMP
			alteracoes.tipo = -1;
			(*PC) = memoria->addr;
			break;
		}
	return (alteracoes);
}

ResulULA ula(int val1, int val2, int ulaOp) {
	ResulULA val;
    switch(ulaOp) {
		case 0:  // ADD/SW/LW/ADDi
			val.resul = val1 + val2;
			break;
		case 2: 
			val.resul = val1 - val2; // SUB
			break;
		case 4: 
			val.resul = val1 & val2; // AND
			break;
		case 5: 
			val.resul = val1 | val2; // OR
			break;
		}
	if(val.resul > -128 && val.resul<127){
		val.OF = 0;
	}else{
		val.OF = 1;
	}
	if(val.resul == 0){
		val.zero = 0;
	}else{
		val.zero = 1;
	}
    return val;
}

// Função para carregar dados de um arquivo externo para a memória
void carregar_memoria(dadosMem* memoriaDados) {
    char nomeArquivo[256];
    int i = 0;
    FILE *arquivo = NULL;
    printf("Nome do arquivo:\t");
    setbuf(stdin, NULL);
    scanf("%s", nomeArquivo);
    //É PRECISO ABRIR O ARQUIVO
    arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL)
		printf("\nFalha ao abrir o arquivo!\n");
	else {		
		while (fscanf(arquivo, "%d", &memoriaDados->dados[i]) != EOF){
			i++;
		}
	}
    fclose(arquivo); //Fecha o arquivo após a leitura
}

void carregaMem(int *contadorInstrucoes, vetorMemoria* memoria){
	FILE* arquivo = NULL;
	char nomeArquivo[256];
	char palavra[17];
	printf("Nome do arquivo:\t");
	setbuf(stdin, NULL);
	scanf("%[^\n]s",nomeArquivo);
	printf("\n");
	getchar();
	if((arquivo = fopen(nomeArquivo,"r")) == NULL ){
		printf("Falha ao abrir arquivo...\n\n");
	} else{
		while(fgets(palavra,(sizeof(palavra)),arquivo) != NULL){
			if(strlen(palavra) == 16 && *contadorInstrucoes < 256){
			printf("\ninstrucao: %s\t", palavra);			
			omi(&memoria[*contadorInstrucoes], palavra); //não precisa do ponteiro pois pode passar o endereço direto da variável (que é um ponteiro)
			palavra[0]='\0'; //só para zerar o vetor de instrução
			printf("opcode: %d\t", memoria[*contadorInstrucoes].opcode);
			printf("RS: \t%d\t", memoria[*contadorInstrucoes].rs);
			printf("RT: \t%d\t", memoria[*contadorInstrucoes].rt);
			printf("RD: \t%d\t", memoria[*contadorInstrucoes].rd);
			printf("Imm: \t%d\t", memoria[*contadorInstrucoes].imm);
			printf("Addr: \t%d\t", memoria[*contadorInstrucoes].addr);
			printf("Funct: \t%d\n", memoria[*contadorInstrucoes].funct);
			(*contadorInstrucoes)++; // Conta para saber quantas instrucoes devem ser lidas na memoria posteriormente...
			}
		}
	}
}

void inicializa_mem(vetorMemoria* memoria){
	for(int i=0; i<256; i++){
		memoria[i].opcode = 0;
		memoria[i].rs = 0;
		memoria[i].rt = 0;
		memoria[i].rd = 0;
		memoria[i].imm = 0;
		memoria[i].addr = 0;
		memoria[i].funct = 0;
	}
}
		
void imprime_mem(vetorMemoria* memoria){
	printf("\n------------------------- Memoria de instrucoes -------------------------\n");
	for(int i=0; i<256 ; i++){
		printf("\ninstrucao: %d\t",i);
		printf("opcode: %d\t", memoria[i].opcode);
		printf("RS: \t%d\t", memoria[i].rs);
		printf("RT: \t%d\t", memoria[i].rt);
		printf("RD: \t%d\t", memoria[i].rd);
		printf("Imm: \t%d\t", memoria[i].imm);
		printf("Addr: \t%d\t", memoria[i].addr);
		printf("Funct: \t%d\n", memoria[i].funct);
	}
}

void inicializa_memdados(dadosMem* memoria){
	for(int i=0; i<256; i++){
		memoria->dados[i] = 0;
	}
}

void voltarInstrucao (back alteracoes, unsigned int* pc, RegistradoresMIPS* mips, dadosMem* memoriaDados){
	(*pc) = alteracoes.pc;
	if(alteracoes.tipo != -1){
		if(alteracoes.tipo == 0)
			mips->registradores[alteracoes.rd] = alteracoes.valRd;
		else
			memoriaDados->dados[alteracoes.mem] = alteracoes.valMem;
	}
}

void salvar_memoria(char filename[], dadosMem memoria[]) {
    FILE *arquivo = fopen(filename, "w"); // Abre o arquivo para escrita
    if (!arquivo) { // Verifica se o arquivo foi aberto corretamente
        perror("Erro ao abrir o arquivo");
        return;
    }
    
    // Escreve os endereços e valores da memória no arquivo
    for (int i = 0; i < MEM_SIZE; i++) {
        fprintf(arquivo, "%d\n", memoria->dados[i]);
    }
    
    fclose(arquivo); // Fecha o arquivo após a escrita
}

#include "lc3.h"
#include <stdio.h> 
#include <stdlib.h>

/*Phansa Chaonpoj
Dongsheng Han
problem 3
*/

// you can define a simple memory module here for this program
unsigned short memory[32];   // 32 words of memory enough to store simple program

//changes data, passed in by reference.
void aluFunction(int opcode, ALU_p alu);
void display(CPU_p cpu, unsigned short memory[], ALU_p alu);
void load(CPU_p cpu, unsigned short memory[], char* filename );
int menuPrompt();
void step(CPU_p cpu, ALU_p alu);
int j=0;
int loaded;
unsigned short pc_temp;// our memory start at 0 always, so it need to minus the beginning pc to work
short starting_address;



int main(int argc, char* argv[]) {
	//making change to code test
 	struct cpu_s cpu1;//creating cpu
 	CPU_p cpu = &cpu1;//creating pointer to cpu

	struct alu_s alus; //creating alu
	ALU_p alu = &alus;


	//setting the register.
	cpu->reg_file[0] = ZERO;
	cpu->reg_file[1] = FIVE;
	cpu->reg_file[2] = FIVETEEN;
	cpu->reg_file[3] = ZERO;
	cpu->reg_file[4] = ZERO;
	cpu->reg_file[5] = ZERO;
	cpu->reg_file[6] = ZERO;
	cpu->reg_file[7] = ZERO;
	cpu->n = 0;
	cpu->z = 0;
	cpu->p = 0;
	cpu->pc = ZERO;
    cpu->ir = ZERO;
    cpu->mar = ZERO;
    cpu->mdr = ZERO;
	alu->a = ZERO;
	alu->b = ZERO;


	//display menu options along with empty display of the system until import the file
	printf("\n%21Welcome to the LC-3 Simulator Simulator\n\n");
	display(cpu, memory, alu);

	int menu = menuPrompt();
	while(menu!=9){
		//load option
		if(menu == 1){
			char filename[30]; //max  
			printf("File name: ");
			if(scanf(" %s", &filename) == 1)
				load(cpu, memory, filename);
				menu = menuPrompt();
		}
		else if(menu == 3){
			if(loaded){
				controller(cpu, alu);
			}else{
				printf("no file loaded\n");
			}
			display(cpu, memory, alu);
			menu = menuPrompt();
		}
		else if(menu == 5){
			printf("Starting Address: 0x");
			scanf(" %X", &starting_address);

			int min = 0x3000;
			int max = 0x3020;

			if(starting_address > max || starting_address < min) 
			{
				printf("Invalid Address. Press <ENTER> to continue.");
				char prev = 0;

		        while(1)
		        {
		            char c = getchar();

		            if(c == '\n' && prev == c)
		            {
		                // double return pressed!
		                break;
		            }

		            prev = c; 
		        }

			}
			else
			{
				display(cpu, memory, alu);
				menu = menuPrompt();
			}
		}
		else if(menu == 9){
			free(cpu);
			free(alu);
			break;
		}else{
			printf("invalid input, quitting....");
			break;
		}
	}
}



int menuPrompt(){
	int menu = 0;
	printf("Select: 1) Load, 3) Step, 5) Display Mem, 9) Exit\n> ");
    scanf(" %d", &menu);
    return menu;
}



/*
Takes in the memory and the string file name.
*/
void load(CPU_p cpu, unsigned short memory[], char* filename ){

	FILE *fp;
	char mem[100]; //for new line 
	fp = fopen(filename , "r");
	if(fp == NULL){

		printf("File not found. Press <ENTER> to continue. \n");
		char prev = 0;

        while(1)
        {
            char c = getchar();

            if(c == '\n' && prev == c)
            {
                // double return pressed!
                return;
            }

            prev = c; 
        }

	}

	
	char* temp;
	fgets (mem, sizeof(mem),fp);
	cpu->pc = strtol(mem, &temp, 16);
	pc_temp = cpu->pc;
	starting_address = pc_temp;
	while( fgets (mem, sizeof(mem),fp)!= NULL ) {
		memory[j++] = strtol(mem, &temp, 16);
	}

	printf("Loading complete. Choose display option to view memory. \n");

	fclose(fp);
	loaded = 1;

}
int controller (CPU_p cpu, ALU_p alu) {
	/*
    struct alu_s alus;
    ALU_p alu = &alus;
	*/

    // check to make sure both pointers are not NULL
	if (cpu == NULL || memory == NULL){
		return 1;
	}
    // do any initializations here
	unsigned int opcode, DR, SR1, SR2;// fields for the IR
	short immed_offset;
	unsigned int state,BEN,n,z,p;
    state = FETCH;
    for (int k = 0;k < j;) {   // efficient endless loop
        switch (state) {
            case FETCH: // microstates 18, 33, 35 in the book
				k++;
				printf("Here in FETCH---------------------------------------------\r\n");
				cpu->mar = cpu->pc;
				cpu->mdr = memory[cpu->pc - pc_temp];
				cpu->pc++;
				cpu->ir = cpu->mdr;
                // get memory[PC] into IR - memory is a global array
                // increment PC
                printf(" Contents of IR = %04X\r\n", cpu->ir);
				//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                // put printf statements in each state and microstate to see that it is working
 				//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				state = DECODE;
				break;
            case DECODE: // microstate 32
				printf("Here in DECODE\r\n");
                // get the fields out of the IR
                // make sure opcode is in integer form
				// hint: use four unsigned int variables, opcode, Rd, Rs, and immed7
				// extract the bit fields from the IR into these variables
				opcode = (cpu->ir & 0xF000) >> 12;
				printf(" Contents of opcode = %4d\r\n", opcode);
                state = EVAL_ADDR;
                break;
            case EVAL_ADDR: // Look at the LD instruction to see microstate 2 example
				printf("Here in EVAL_ADDR\r\n");
                switch (opcode) {
					case JMP :
						printf(" here in JMP\r\n");
						SR1 = (cpu->ir & 0x01C0) >> 6;
						printf(" BaseR = %4d\r\n", SR1);
						cpu->pc = cpu->reg_file[SR1];
						printf(" Contents of PC = %4d\r\n", cpu->pc);
						break;
					case LD :
						printf(" here in LD\r\n");
						cpu->mar = cpu->pc + (cpu->ir & 0x01FF);
						printf(" Contents of MAR = %4d\r\n", cpu->mar);
						break;
					case TRAP :
						printf(" here in TRAP\r\n");
						cpu->mar = cpu->ir & 0x00FF;
					    printf(" Contents of MAR = %4d\r\n", cpu->mar);
						break;
					case ST :
						printf(" here in ST\r\n");
						cpu->mar = cpu->ir & 0x001F;
						printf(" Contents of MAR = %4d\r\n", cpu->mar);
						cpu->mar += pc_temp;
						break;
					default :
						printf(" here in default\r\n");
                // different opcodes require different handling
                // compute effective address, e.g. add sext(immed7) to register
				}
                state = FETCH_OP;
                break;
            case FETCH_OP: // Look at ST. Microstate 23 example of getting a value out of a register
				printf("Here in FETCH_OP\r\n");
				switch (opcode) {
                    // get operands out of registers into A, B of ALU
                    // or get memory for load instr.
					case ADD:
						printf(" here in ADD\r\n");
						DR = (cpu->ir & 0x0E00) >> 9;
						printf(" Contents of DR = %4d\r\n", DR);
						SR1 = (cpu->ir & 0x01C0) >> 6;
						printf(" Contents of SR1 = %4d\r\n", SR1);
						cpu->cc = (cpu->ir & 0x0020) >> 5;
						
						alu->a = cpu->reg_file[SR1];
						if(cpu->cc){
							immed_offset = cpu->ir & 0x001F;
							if(immed_offset & 0x0010 >> 4){
								immed_offset += 0xFFE0;
							}
							alu->b = immed_offset;
							printf(" Contents of imm5 = %4d\r\n", immed_offset);
						}else{
							SR2 = cpu->ir & 0x0007;
							alu->b = cpu->reg_file[SR2];
							printf(" Contents of SR2 = %4d\r\n", SR2);
						}
						break;
					case AND:
						printf(" here in AND\r\n");
						DR = (cpu->ir & 0x0E00) >> 9;
						printf(" Contents of DR = %4d\r\n", DR);
						SR1 = (cpu->ir & 0x01C0) >> 6;
						printf(" Contents of SR1 = %4d\r\n", SR1);
						cpu->cc = (cpu->ir & 0x0020) >> 5;
						
						alu->a = cpu->reg_file[SR1];
						if(cpu->cc){
							immed_offset = cpu->ir & 0x001F;
							alu->b = immed_offset;
							printf(" Contents of imm5 = %4d\r\n", immed_offset);
						}else{
							SR2 = cpu->ir & 0x0007;
							alu->b = cpu->reg_file[SR2];
							printf(" Contents of SR2 = %4d\r\n", SR2);
						}
						break;
					case NOT:
						printf(" here in NOT\r\n");
						DR = (cpu->ir & 0x0E00) >> 9;
						printf(" Contents of DR = %4d\r\n", DR);
						SR1 = (cpu->ir & 0x01C0) >> 6;
						printf(" Contents of SR = %4d\r\n", SR1);
						alu->a = cpu->reg_file[SR1];
						break;
					case LD:
						printf(" here in LD\r\n");
						cpu->mdr = memory[cpu->mar - pc_temp];
						printf(" Contents of MDR = %4d\r\n", cpu->mdr);
						DR = (cpu->ir & 0x0E00) >> 9;
						printf(" Contents of DR = %4d\r\n", DR);
						break;
					case BR:
						printf(" here in BR\r\n");
						n = cpu->ir & 0x0800 >> 11;
						z = cpu->ir & 0x0400 >> 10;
						p = cpu->ir & 0x0200 >> 9;
						BEN = cpu->n & n + cpu->z & z + cpu->p & p;
						printf(" Contents of BEN = %4d\r\n", BEN);
						if (BEN == 1) {
							cpu->pc = cpu->pc + (cpu->ir & 0x001F);
							printf(" Contents of PC = %4d\r\n", cpu->pc);
						}
						break;
					case ST:
						printf(" here in ST\r\n");
						SR1 = (cpu->ir & 0x0E00) >> 9;
						printf(" Contents of SR = %4d\r\n", SR1);
						cpu->mdr = cpu->reg_file[SR1];
						printf(" Contents of MDR = %4d\r\n", cpu->mdr);
						break;
					default:
						printf(" here in default\r\n");
				}
                state = EXECUTE;
                break;
            case EXECUTE: // Note that ST does not have an execute microstate
				printf("Here in EXECUTE\r\n");
                switch (opcode) {
                    // do what the opcode is for, e.g. add
                    // in case of trap: call trap(int trap_vector) routine, see below for trap x25 (halt)
					case ADD:
						printf(" here in ADD\r\n");
						aluFunction(ADD, alu);
						cpu->reg_file[DR] = alu->r;
						printf(" Contents of DR = %4d\r\n", cpu->reg_file[DR]);
						
						cpu->n = 0;
						cpu->z = 0;
						cpu->p = 0;
						if (cpu->reg_file[DR] > 0) {
							cpu->p = 1;
							printf(" Contents of  P = %4d\r\n", cpu->p);
						}
						else if (cpu->reg_file[DR] == 0) {
							cpu->z = 1;
							printf(" Contents of  Z = %4d\r\n", cpu->z);
						}
						else if (cpu->reg_file[DR] < 0) {
							cpu->n = 1;
							printf(" Contents of N = %4d\r\n", cpu->n);
						}
						break;
					case AND:
						printf(" here in AND\r\n");
						aluFunction(AND, alu);
						cpu->reg_file[DR] = alu->r;
						printf(" Contents of DR = %4d\r\n", cpu->reg_file[DR]);
						
						cpu->n = 0;
						cpu->z = 0;
						cpu->p = 0;
						if (cpu->reg_file[DR] > 0) {
							cpu->p = 1;
							printf(" Contents of  P = %4d\r\n", cpu->p);
						}
						else if (cpu->reg_file[DR] == 0) {
							cpu->z = 1;
							printf(" Contents of  Z = %4d\r\n", cpu->z);
						}
						else if (cpu->reg_file[DR] < 0) {
							cpu->n = 1;
							printf(" Contents of  N = %4d\r\n", cpu->n);
						}
						break;
					case NOT:
						printf(" here in NOT\r\n");
						aluFunction(NOT, alu);
						cpu->reg_file[DR] = alu->r;
						printf(" Contents of DR = %4d\r\n", cpu->reg_file[DR]);
						
						cpu->n = 0;
						cpu->z = 0;
						cpu->p = 0;
						if (cpu->reg_file[DR] > 0) {
							cpu->p = 1;
							printf(" Contents of  P = %4d\r\n", cpu->p);
						}
						else if (cpu->reg_file[DR] == 0) {
							cpu->z = 1;
							printf(" Contents of  Z = %4d\r\n", cpu->z);
						}
						else if (cpu->reg_file[DR] < 0) {
							cpu->n = 1;
							printf(" Contents of  N = %4d\r\n", cpu->n);
						}
						break;
					case LD:
						printf(" here in LD\r\n");
						cpu->reg_file[DR] = cpu->mdr;
						printf(" Contents of DR = %4d\r\n", cpu->reg_file[DR]);
						
						cpu->n = 0;
						cpu->z = 0;
						cpu->p = 0;
						if (cpu->reg_file[DR] > 0) {
							cpu->p = 1;
							printf(" Contents of  P = %4d\r\n", cpu->p);
						}
						else if (cpu->reg_file[DR] == 0) {
							cpu->z = 1;
							printf(" Contents of  Z = %4d\r\n", cpu->z);
						}
						else if (cpu->reg_file[DR] < 0) {
							cpu->n = 1;
							printf(" Contents of  N = %4d\r\n", cpu->n);
						}
						break;
					default:
						printf(" here in default\r\n");
                }
                state = STORE;
                break;
            case STORE: // Look at ST. Microstate 16 is the store to memory
				printf("Here in STORE\r\n");
                switch (opcode) {
                    // write back to register or store MDR into memory
					case ST:
						printf(" here in ST\r\n");
						memory[cpu->mar - pc_temp] = cpu->mdr;
						printf(" Contents of memory[MAR] = %4d\r\n", memory[cpu->mar - pc_temp]);
						break;
					default:
						printf(" here in default\r\n");
                }
                // do any clean up here in prep for the next complete cycle
                state = FETCH;
                break;
        }
    }
}

/* Passed in by reference of alu. */
void aluFunction(int opcode, ALU_p alu){

	if(opcode == ADD){
		alu->r = alu->a + alu->b ;
	}else if(opcode == AND){
		//temp for bit shifting 
		alu->r = alu->a & alu->b; 
	}else if(opcode == NOT){
		alu->r = ~(alu->a);
		//if it's in register b then use this '
		//alu->r = ~(alu->b);
	}


}

void display(CPU_p cpu, unsigned short memory[], ALU_p alu){

	//display registers and memory 
	printf("%21Registor%21Memory\n");
	int i = 0;
	int k = 16;
	for( ; i < k; i++){
		if(i <= 7){
			printf("%20s", "");
			printf("R%d: x%08X", i , cpu->reg_file[i]);
			printf("%13s", "");
			printf("x%04x: x%04X\n", starting_address + i, memory[starting_address - pc_temp + i]);
		}else if(i == 11){
			printf("%20s", "");
			printf("PC: x%04X%3IR: x%04X", cpu->pc, cpu->ir);
			printf("%6s", "");
			printf("x%04X: x%04X\n", starting_address + i, memory[starting_address - pc_temp + i]);
		}else if(i == 12){
			printf("%20s", "");
			printf("A:  x%04X%3B:  x%04X", alu->a, alu->b );
			printf("%6s", "");
			printf("x%04X: x%04X\n", starting_address + i, memory[starting_address - pc_temp + i]);
		}else if(i == 13){
			printf("%20s", "");
			printf("MAR:x%04X%3MDR:x%04X", cpu->mar, cpu->mdr);
			printf("%6s", "");
			printf("x%04X: x%04X\n", starting_address + i, memory[starting_address - pc_temp + i]);
		}else if(i == 14){
			printf("%20s", "");
			printf("CC: %i%2N: %i%2 Z: %i%2P: %i", cpu->cc, cpu->n, cpu->z, cpu->p);
			printf("%6s", "");
			printf("x%04X: x%04X\n", starting_address + i, memory[starting_address - pc_temp + i]);
		}else{
			printf("%46s", "");
			printf("x%04X: x%04X\n", starting_address + i, memory[starting_address - pc_temp + i]);
		}	
	}
}


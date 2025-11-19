/*
 * trace.c: location of main() to start the simulator
 */

#include "loader.h"

int main(int argc, char** argv)
{
		printf("TEST\n");
    // HOW to allocate memory for your CPU:

    // 1) look in the file: LC4.h, examine the structure called: "MachineState"
    // 2) Create a local variable of type "MachineState" here in main()
    // 3) This local variable represents the entire LC4 CPU - it's registers, control signals, and all of its memory
    // 4) You can now pass a pointer to your local variable to the different functions (ex: ReadObjectFile())
    // 5) DO NOT USE malloc() or free() at all in this assignment! - you will lose points if you do

    if(argc < 3){
        printf("not enough arguments\n");
        return -1;
    }

    MachineState cpu;
    memset(&cpu, 0, sizeof(MachineState));

    for(int i = 2; i < argc; i++){
        if(ReadObjectFile(argv[i], &cpu) != 0){
            printf("can't open file: %s\n", argv[i]);
            return -1;
        }
    }

    FILE* out = fopen(argv[1], "w");
    if(out == NULL){
        printf("can't open file: %s\n", argv[1]);
        return -1;
    }

    // for(int i = 0; i < 65536; i++){
    //     if(cpu.memory[i] != 0){
    //         fprintf(out, "address: %05d contents: 0x%04X\n", i, cpu.memory[i]);
    //     }
    // }

		Reset(&cpu);
		printf("starting simulation\n");
		while(1){
			// WriteOut(&cpu, out);
			// if(cpu.PC == 0x820E){
			// 	printf("%04X | %d\n", cpu.PC, cpu.memory[cpu.PC]);
			// 	UpdateMachineState(&cpu, out);
			// 	printf("%04X | %d\n", cpu.PC, cpu.memory[cpu.PC]);
			// 	exit(1);
			// }
			// if(cpu.PC == 0x820E){
			// 	printf("%04X ", cpu.memory[cpu.PC]);
			// }
			if(UpdateMachineState(&cpu, out) == -1){
				break;
			}
		}
		printf("end of simulation\n");
    
    fclose(out);
    return 0;
}
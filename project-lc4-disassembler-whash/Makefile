all: lc4 

lc4: lc4.o lc4_disassembler.o lc4_hash.o lc4_loader.o lc4_memory.o
	clang -Wall -g lc4.o lc4_disassembler.o lc4_hash.o lc4_loader.o lc4_memory.o -o lc4 

lc4.o: lc4.c lc4_disassembler.h lc4_hash.h lc4_loader.h lc4_memory.h
	clang -Wall -g -c lc4.c -o lc4.o

lc4_disassembler.o: lc4_disassembler.c lc4_disassembler.h lc4_hash.h
	clang -Wall -g -c lc4_disassembler.c -o lc4_disassembler.o

lc4_hash.o: lc4_hash.c lc4_hash.h lc4_memory.h
	clang -Wall -g -c lc4_hash.c -o lc4_hash.o

lc4_loader.o: lc4_loader.c lc4_loader.h lc4_hash.h lc4_memory.h
	clang -Wall -g -c lc4_loader.c -o lc4_loader.o

lc4_memory.o: lc4_memory.c lc4_memory.h 
	clang -Wall -g -c lc4_memory.c -o lc4_memory.o

clean:
	rm -rf *.o 

clobber: clean 
	rm -rf lc4
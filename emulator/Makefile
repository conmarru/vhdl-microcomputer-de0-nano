CFLAGS = -fno-common -O3 -Wall -Wextra

emu: main.o disasm.o
	gcc main.o disasm.o $(CFLAGS) -o $@

main.o: main.c 
	gcc -c $(CFLAGS) main.c

disasm.o: disasm.c
	gcc -c $(CFLAGS) disasm.c

clean: 
	rm *.o; rm emu

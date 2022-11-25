CFLAGS = -fno-common -O3 -Wall -Wextra

SRC_DIR = src
HEADER_DIR = headers

emu: main.o disasm.o cmds.o utils.o
	gcc main.o disasm.o cmds.o utils.o $(CFLAGS) -o $@

main.o: $(SRC_DIR)/main.c 
	gcc -c $(CFLAGS) $(SRC_DIR)/main.c

disasm.o: $(SRC_DIR)/disasm.c
	gcc -c $(CFLAGS) $(SRC_DIR)/disasm.c

cmds.o: $(SRC_DIR)/cmds.c
	gcc -c $(CFLAGS) $(SRC_DIR)/cmds.c

utils.o : $(SRC_DIR)/utils.c
	gcc -c $(CFLAGS) $(SRC_DIR)/utils.c

clean: 
	rm *.o; rm emu

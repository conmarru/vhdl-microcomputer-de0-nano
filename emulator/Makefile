CFLAGS = -fno-common -O3 -Wall -Wextra -pedantic -Wno-unused-functions -Wno-unused-parameter -std=c11
MACRO_DEFS = -D_POSIX_C_SOURCE
CC = gcc

SRC_DIR = src
HEADER_DIR = headers
TARGETS = main.o disasm.o cmds.o utils.o fetch.o decode.o exec.o mem.o wrback.o pipeline.o seq.o

emu: $(TARGETS)
	$(CC) $(TARGETS) $(CFLAGS) -o $@

main.o: $(SRC_DIR)/main.c 
	$(CC) -c $(CFLAGS) $(MACRO_DEFS) $(SRC_DIR)/main.c

disasm.o: $(SRC_DIR)/disasm.c
	$(CC) -c $(CFLAGS) $(MACRO_DEFS) $(SRC_DIR)/disasm.c

cmds.o: $(SRC_DIR)/cmds.c
	$(CC) -c $(CFLAGS) $(MACRO_DEFS) $(SRC_DIR)/cmds.c

utils.o: $(SRC_DIR)/utils.c
	$(CC) -c $(CFLAGS) $(MACRO_DEFS) $(SRC_DIR)/utils.c

fetch.o: $(SRC_DIR)/fetch.c
	$(CC) -c $(CFLAGS) $(MACRO_DEFS) $(SRC_DIR)/fetch.c

decode.o: $(SRC_DIR)/decode.c
	$(CC) -c $(CFLAGS) $(MACRO_DEFS) $(SRC_DIR)/decode.c

exec.o: $(SRC_DIR)/exec.c
	$(CC) -c $(CFLAGS) $(MACRO_DEFS) $(SRC_DIR)/exec.c

mem.o: $(SRC_DIR)/mem.c
	$(CC) -c $(CFLAGS) $(MACRO_DEFS) $(SRC_DIR)/mem.c

wrback.o: $(SRC_DIR)/wrback.c
	$(CC) -c $(CFLAGS) $(MACRO_DEFS) $(SRC_DIR)/wrback.c

pipeline.o: $(SRC_DIR)/pipeline.c
	$(CC) -c $(CFLAGS) $(MACRO_DEFS) $(SRC_DIR)/pipeline.c

seq.o: $(SRC_DIR)/seq.c
	$(CC) -c $(CFLAGS) $(MACRO_DEFS) $(SRC_DIR)/seq.c

clean: 
	rm -f *.o; rm -f emu

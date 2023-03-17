CFLAGS = -fno-common -O3 -Wall -Wextra -pedantic -Wno-unused-function -Wno-unused-parameter -std=c11
MACRO_DEFS = -D_POSIX_C_SOURCE=200809L
PREPROC_FLAGS = -DLBL_HASHSET_SIZE=20 -DKYWRD_HASHSET_SIZE=30
CC = gcc

ifdef debug
	PREPROC_FLAGS += -DDEBUG
endif

SRC_DIR = src
HEADER_DIR = header
TARGETS = main.o scan.o gen.o utils.o eval.o lbl.o parser.o instr.o

asm: $(TARGETS)
	$(CC) $(TARGETS) $(CFLAGS) -o $@

main.o: $(SRC_DIR)/main.c 
	$(CC) -c $(CFLAGS) $(MACRO_DEFS) $(PREPROC_FLAGS) $(SRC_DIR)/main.c

scan.o: $(SRC_DIR)/scan.c
	$(CC) -c $(CFLAGS) $(MACRO_DEFS) $(PREPROC_FLAGS) $(SRC_DIR)/scan.c

gen.o: $(SRC_DIR)/gen.c
	$(CC) -c $(CFLAGS) $(MACRO_DEFS) $(PREPROC_FLAGS) $(SRC_DIR)/gen.c

utils.o: $(SRC_DIR)/utils.c
	$(CC) -c $(CFLAGS) $(MACRO_DEFS) $(PREPROC_FLAGS) $(SRC_DIR)/utils.c

lbl.o: $(SRC_DIR)/lbl.c
	$(CC) -c $(CFLAGS) $(MACRO_DEFS) $(PREPROC_FLAGS) $(SRC_DIR)/lbl.c

eval.o: $(SRC_DIR)/eval.c
	$(CC) -c $(CFLAGS) $(MACRO_DEFS) $(PREPROC_FLAGS) $(SRC_DIR)/eval.c

parser.o: $(SRC_DIR)/parser.c
	$(CC) -c $(CFLAGS) $(MACRO_DEFS) $(PREPROC_FLAGS) $(SRC_DIR)/parser.c

instr.o: $(SRC_DIR)/instr.c
	$(CC) -c $(CFLAGS) $(MACRO_DEFS) $(PREPROC_FLAGS) $(SRC_DIR)/instr.c

test: 
	./run-tests.sh

clean: 
	rm -f *.o; rm -f asm

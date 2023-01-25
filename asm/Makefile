CFLAGS = -fno-common -O3 -Wall -Wextra

SRC_DIR = src
HEADER_DIR = header
TARGETS = main.o scan.o gen.o utils.o eval.o lbl.o parser.o instr.o

asm: $(TARGETS)
	gcc $(TARGETS) $(CFLAGS) -o $@

main.o: $(SRC_DIR)/main.c 
	gcc -c $(CFLAGS) $(SRC_DIR)/main.c

scan.o: $(SRC_DIR)/scan.c
	gcc -c $(CFLAGS) $(SRC_DIR)/scan.c

gen.o: $(SRC_DIR)/gen.c
	gcc -c $(CFLAGS) $(SRC_DIR)/gen.c

utils.o: $(SRC_DIR)/utils.c
	gcc -c $(CFLAGS) $(SRC_DIR)/utils.c

lbl.o: $(SRC_DIR)/lbl.c
	gcc -c $(CFLAGS) $(SRC_DIR)/lbl.c

eval.o: $(SRC_DIR)/eval.c
	gcc -c $(CFLAGS) $(SRC_DIR)/eval.c

parser.o: $(SRC_DIR)/parser.c
	gcc -c $(CFLAGS) $(SRC_DIR)/parser.c

instr.o: $(SRC_DIR)/instr.c
	gcc -c $(CFLAGS) $(SRC_DIR)/instr.c

test: 
	./run-tests.sh

clean: 
	rm -f *.o; rm -f asm

CFLAGS = -fno-common -O3 -Wall -Wextra 

SRC_DIR = src
HEADER_DIR = header

asm: main.o scan.o gen.o utils.o
	gcc main.o scan.o gen.o utils.o $(CFLAGS) -o $@

main.o: $(SRC_DIR)/main.c 
	gcc -c $(CFLAGS) $(SRC_DIR)/main.c

scan.o : $(SRC_DIR)/scan.c
	gcc -c $(CFLAGS) $(SRC_DIR)/scan.c

gen.o : $(SRC_DIR)/gen.c
	gcc -c $(CFLAGS) $(SRC_DIR)/gen.c

utils.o : $(SRC_DIR)/utils.c
	gcc -c $(CFLAGS) $(SRC_DIR)/utils.c

test : 
	./main test.txt

clean: 
	rm *.o; rm asm

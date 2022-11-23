CFLAGS = -fno-common -O3 -Wall -Wextra 

SRC_DIR = src
HEADER_DIR = header

asm: main.o scan.o gen.o
	gcc main.o scan.o gen.o $(CFLAGS) -o $@

main.o: $(SRC_DIR)/main.c 
	gcc -c $(CFLAGS) $(SRC_DIR)/main.c

scan.o : $(SRC_DIR)/scan.c
	gcc -c $(CFLAGS) $(SRC_DIR)/scan.c

gen.o : $(SRC_DIR)/gen.c
	gcc -c $(CFLAGS) $(SRC_DIR)/gen.c

test : 
	./main test.txt

clean: 
	rm *.o; rm asm

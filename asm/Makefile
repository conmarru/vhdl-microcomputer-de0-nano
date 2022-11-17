CFLAGS = -fno-common -O3 -Wall

SRC_DIR = src
HEADER_DIR = header

main: main.o scan.o
	gcc main.o scan.o $(CFLAGS) -o main

main.o: $(SRC_DIR)/main.c 
	gcc -c $(CFLAGS) $(SRC_DIR)/main.c

scan.o : $(SRC_DIR)/scan.c
	gcc -c $(CFLAGS) $(SRC_DIR)/scan.c

clean: 
	rm *.o; rm main

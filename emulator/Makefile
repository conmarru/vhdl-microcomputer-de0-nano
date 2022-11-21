CFLAGS = -fno-common -O3 -Wall -Wextra

main: main.o
	gcc main.o $(CFLAGS) -o $@

main.o: main.c
	gcc -c $(CFLAGS) main.c

clean: 
	rm *.o; rm main

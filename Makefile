PROGS = asm emulator

all:
	for dir in $(PROGS); do \
		(cd $$dir; ${MAKE}); \
		done

clean:
	for dir in $(PROGS); do \
		(cd $$dir; ${MAKE} clean); \
		done

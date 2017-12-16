LABY_GENERATOR_C_FLAGS=-O2 -std=c99 -Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wreturn-type -Wshadow -Wstrict-prototypes -Wswitch -Wwrite-strings

laby_generator: laby_generator.o
	gcc -o laby_generator laby_generator.o

laby_generator.o: laby_generator.c laby_generator.make
	gcc -c ${LABY_GENERATOR_C_FLAGS} -o laby_generator.o laby_generator.c

clean:
	rm -f laby_generator laby_generator.o

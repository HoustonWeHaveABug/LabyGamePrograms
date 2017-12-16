LABY_SOLVER_C_FLAGS=-O2 -std=c99 -Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wreturn-type -Wshadow -Wstrict-prototypes -Wswitch -Wwrite-strings

laby_solver: laby_solver.o
	gcc -o laby_solver laby_solver.o

laby_solver.o: laby_solver.c laby_solver.make
	gcc -c ${LABY_SOLVER_C_FLAGS} -o laby_solver.o laby_solver.c

clean:
	rm -f laby_solver laby_solver.o

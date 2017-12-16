#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define OPERATIONS_N 4

void print_row(void);
void print_value(int);
void print_operator(int);
int erand(int, int);

int operations[OPERATIONS_N] = { '+', '*', '-', '/' }, columns_n, value_min, value_max, frequencies[OPERATIONS_N];

int main(void) {
	int rows_n, row_first, column_first, goal, row, column, i;
	if (scanf("%d%d%d%d%d%d%d%d%d%d%d", &rows_n, &columns_n, &value_min, &value_max, frequencies, frequencies+1, frequencies+2, frequencies+3, &row_first, &column_first, &goal) != 7+OPERATIONS_N || rows_n < 1 || columns_n < 1 || value_min > value_max || value_max-value_min+1 > RAND_MAX || frequencies[0] < 0 || frequencies[1] < 0 || frequencies[2] < 0 || frequencies[3] < 0 || row_first < 0 || row_first >= rows_n || column_first < 0 || column_first >= columns_n) {
		fprintf(stderr, "Invalid parameters\n");
		fflush(stderr);
	}
	for (i = 1; i < OPERATIONS_N; i++) {
		frequencies[i] += frequencies[i-1];
	}
	if (frequencies[OPERATIONS_N-1] == 0 || frequencies[OPERATIONS_N-1] > RAND_MAX) {
		fprintf(stderr, "Invalid frequencies (sum = 0 or greater than RAND_MAX)\n");
		fflush(stderr);
	}
	srand((unsigned)time(NULL));
	printf("%d %d\n", rows_n, columns_n);
	print_row();
	for (row = 1; row < rows_n; row++) {
		for (column = 0; column < columns_n-1; column++) {
			print_operator(' ');
		}
		print_operator('\n');
		print_row();
	}
	printf("%d %d\n%d\n", row_first, column_first, goal);
	fflush(stdout);
	return EXIT_SUCCESS;
}

void print_row(void) {
	int column;
	for (column = 0; column < columns_n-1; column++) {
		print_value(' ');
		print_operator(' ');
	}
	print_value('\n');
}

void print_value(int separator) {
	printf("%d%c", erand(value_max-value_min+1, value_min), separator);
}

void print_operator(int separator) {
	int frequency = erand(frequencies[OPERATIONS_N-1], 0), i;
	for (i = 0; i < OPERATIONS_N && frequencies[i] <= frequency; i++);
	printf("%c%c", operations[i], separator);
}

int erand(int range, int offset) {
	return (int)(rand()/(RAND_MAX+1.0)*range)+offset;
}

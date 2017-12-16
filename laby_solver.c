#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <time.h>

#define DIRECTIONS_N 4
#define DIRECTION_WEST 0
#define DIRECTION_NORTH 1
#define DIRECTION_EAST 2
#define DIRECTION_SOUTH 3
#define OPERATION_NONE 0
#define OPERATION_ADD '+'
#define OPERATION_MULTIPLY '*'
#define OPERATION_SUBTRACT '-'
#define OPERATION_DIVIDE '/'

typedef struct {
	int row;
	int column;
	int value;
	int operations[DIRECTIONS_N];
}
cell_t;

typedef enum {
	CALL_TYPE_ROLL,
	CALL_TYPE_UNROLL
}
call_type_t;

typedef struct {
	call_type_t type;
	int choices_idx_or_value;
	cell_t *cell;
	int direction_inv;
}
call_t;

typedef enum {
	CALL_RESULT_FAILURE,
	CALL_RESULT_SUCCESS,
	CALL_RESULT_FOUND
}
call_result_t;

int read_row(cell_t *, int);
int read_value(cell_t *, int, int);
int read_operation(cell_t *, int, cell_t *, int);
int add_call(call_type_t, int, cell_t *, int);
void set_call(call_t *, call_type_t, int, cell_t *, int);
call_result_t perform_call(call_t *);
call_result_t perform_roll(int, cell_t *, int);
void print_cell(cell_t *, int);
call_result_t perform_unroll(int, cell_t *, int);
int test_direction(cell_t *, int, cell_t *);
void perform_operation(cell_t *, int, cell_t *, int, int *);
void free_data(void);

int columns_n, goal, calls_max, calls_size, choices_max;
unsigned time0;
unsigned long long nodes;
cell_t *cells, **choices;
call_t *calls;

int main(void) {
	int rows_n, cells_n, row, column, r;
	cell_t *cell;
	if (scanf("%d%d", &rows_n, &columns_n) != 2 || rows_n < 1 || columns_n < 1) {
		fprintf(stderr, "Invalid grid size\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}
	cells_n = rows_n*columns_n;
	cells = calloc((size_t)cells_n, sizeof(cell_t));
	if (!cells) {
		fprintf(stderr, "Could not allocate memory for cells\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}
	if (!read_row(cells, 0)) {
		free(cells);
		return EXIT_FAILURE;
	}
	for (cell = cells, row = 1; row < rows_n; row++) {
		for (column = 0; column < columns_n; cell++, column++) {
			if (!read_operation(cell, DIRECTION_SOUTH, cell+columns_n, DIRECTION_NORTH)) {
				free(cells);
				return EXIT_FAILURE;
			}
		}
		if (!read_row(cell, row)) {
			free(cells);
			return EXIT_FAILURE;
		}
	}
	if (scanf("%d%d", &row, &column) != 2 || row < 0 || row >= rows_n || column < 0 || column >= columns_n) {
		fprintf(stderr, "Invalid starting cell\n");
		fflush(stderr);
		free(cells);
		return EXIT_FAILURE;
	}
	if (scanf("%d", &goal) != 1) {
		fprintf(stderr, "Invalid goal\n");
		fflush(stderr);
		free(cells);
		return EXIT_FAILURE;
	}
	calls_max = 0;
	calls_size = 0;
	choices_max = 0;
	nodes = 0;
	time0 = (unsigned)time(NULL);
	do {
		if (choices_max == 0) {
			choices = malloc(sizeof(cell_t *));
			if (!choices) {
				fprintf(stderr, "Could not allocate memory for choices\n");
				fflush(stderr);
				free_data();
				return EXIT_FAILURE;
			}
		}
		else {
			cell_t **choices_tmp = realloc(choices, sizeof(cell_t *)*(size_t)(choices_max+1));
			if (!choices_tmp) {
				fprintf(stderr, "Could not reallocate memory for choices\n");
				fflush(stderr);
				free_data();
				return EXIT_FAILURE;
			}
			choices = choices_tmp;
		}
		choices_max++;
		printf("Length %d\n", choices_max);
		fflush(stdout);
		if (!add_call(CALL_TYPE_ROLL, 0, cells+row*columns_n+column, DIRECTIONS_N)) {
			free_data();
			return EXIT_FAILURE;
		}
		do {
			calls_size--;
			r = perform_call(calls+calls_size);
		}
		while (calls_size > 0 && r == CALL_RESULT_SUCCESS);
	}
	while (r == CALL_RESULT_SUCCESS);
	if (r == CALL_RESULT_FOUND) {
		printf("Nodes %llu Time %us\n", nodes, (unsigned)time(NULL)-time0);
		fflush(stdout);
	}
	free_data();
	return EXIT_SUCCESS;
}

int read_row(cell_t *cell, int row) {
	int column;
	if (!read_value(cell, row, 0)) {
		return 0;
	}
	for (cell++, column = 1; column < columns_n; cell++, column++) {
		if (!read_operation(cell-1, DIRECTION_EAST, cell, DIRECTION_WEST)) {
			return 0;
		}
		if (!read_value(cell, row, column)) {
			return 0;
		}
	}
	return 1;
}

int read_value(cell_t *cell, int row, int column) {
	if (scanf("%d", &cell->value) != 1) {
		fprintf(stderr, "Invalid cell value\n");
		fflush(stderr);
		return 0;
	}
	cell->row = row;
	cell->column = column;
	return 1;
}

int read_operation(cell_t *cell_a, int direction_a, cell_t *cell_b, int direction_b) {
	int operation;
	do {
		operation = fgetc(stdin);
	}
	while (isspace(operation));
	if (operation != OPERATION_ADD && operation != OPERATION_MULTIPLY && operation != OPERATION_SUBTRACT && operation != OPERATION_DIVIDE) {
		fprintf(stderr, "Invalid operation\n");
		fflush(stderr);
		return 0;
	}
	cell_a->operations[direction_a] = operation;
	cell_b->operations[direction_b] = operation;
	return 1;
}

int add_call(call_type_t call_type, int choices_idx, cell_t *cell, int direction_inv) {
	if (calls_size == calls_max) {
		if (calls_max == 0) {
			calls = malloc(sizeof(call_t));
			if (!calls) {
				fprintf(stderr, "Could not allocate memory for calls\n");
				fflush(stderr);
				return 0;
			}
		}
		else {
			call_t *calls_tmp = realloc(calls, sizeof(call_t)*(size_t)(calls_max+1));
			if (!calls_tmp) {
				fprintf(stderr, "Could not reallocate memory for calls\n");
				fflush(stderr);
				return 0;
			}
			calls = calls_tmp;
		}
		calls_max++;
	}
	set_call(calls+calls_size, call_type, choices_idx, cell, direction_inv);
	calls_size++;
	return 1;
}

void set_call(call_t *call, call_type_t type, int choices_idx_or_value, cell_t *cell, int direction_inv) {
	call->type = type;
	call->choices_idx_or_value = choices_idx_or_value;
	call->cell = cell;
	call->direction_inv = direction_inv;
}

call_result_t perform_call(call_t *call) {
	call_result_t r;
	switch (call->type) {
	case CALL_TYPE_ROLL:
		r = perform_roll(call->choices_idx_or_value, call->cell, call->direction_inv);
		break;
	case CALL_TYPE_UNROLL:
		r = perform_unroll(call->choices_idx_or_value, call->cell, call->direction_inv);
		break;
	default:
		fprintf(stderr, "Invalid call\n");
		fflush(stderr);
		r = CALL_RESULT_FAILURE;
	}
	return r;
}

call_result_t perform_roll(int choices_idx, cell_t *cell, int direction_inv) {
int value_a;
	if (choices_idx == choices_max) {
		return CALL_RESULT_SUCCESS;
	}
	nodes++;
	choices[choices_idx] = cell;
	if (direction_inv == DIRECTION_WEST) {
		perform_operation(cell-1, DIRECTION_EAST, cell, DIRECTION_WEST, &value_a);
	}
	else if (direction_inv == DIRECTION_NORTH) {
		perform_operation(cell-columns_n, DIRECTION_SOUTH, cell, DIRECTION_NORTH, &value_a);
	}
	else if (direction_inv == DIRECTION_EAST) {
		perform_operation(cell+1, DIRECTION_WEST, cell, DIRECTION_EAST, &value_a);
	}
	else if (direction_inv == DIRECTION_SOUTH) {
		perform_operation(cell+columns_n, DIRECTION_NORTH, cell, DIRECTION_SOUTH, &value_a);
	}
	if (cell->value == goal) {
		int i;
		for (i = 0; i < choices_idx; i++) {
			print_cell(choices[i], ' ');
		}
		print_cell(choices[i], '\n');
		fflush(stdout);
		return CALL_RESULT_FOUND;
	}
	if (!add_call(CALL_TYPE_UNROLL, value_a, cell, direction_inv)) {
		return CALL_RESULT_FAILURE;
	}
	if (direction_inv != DIRECTION_WEST && cell->operations[DIRECTION_WEST] != OPERATION_NONE && test_direction(cell, DIRECTION_WEST, cell-1) && !add_call(CALL_TYPE_ROLL, choices_idx+1, cell-1, DIRECTION_EAST)) {
		return CALL_RESULT_FAILURE;
	}
	if (direction_inv != DIRECTION_NORTH && cell->operations[DIRECTION_NORTH] != OPERATION_NONE && test_direction(cell, DIRECTION_NORTH, cell-columns_n) && !add_call(CALL_TYPE_ROLL, choices_idx+1, cell-columns_n, DIRECTION_SOUTH)) {
		return CALL_RESULT_FAILURE;
	}
	if (direction_inv != DIRECTION_EAST && cell->operations[DIRECTION_EAST] != OPERATION_NONE && test_direction(cell, DIRECTION_EAST, cell+1) && !add_call(CALL_TYPE_ROLL, choices_idx+1, cell+1, DIRECTION_WEST)) {
		return CALL_RESULT_FAILURE;
	}
	if (direction_inv != DIRECTION_SOUTH && cell->operations[DIRECTION_SOUTH] != OPERATION_NONE && test_direction(cell, DIRECTION_SOUTH, cell+columns_n) && !add_call(CALL_TYPE_ROLL, choices_idx+1, cell+columns_n, DIRECTION_NORTH)) {
		return CALL_RESULT_FAILURE;
	}
	return CALL_RESULT_SUCCESS;
}

void print_cell(cell_t *cell, int separator) {
	printf("(%d, %d)%c", cell->row, cell->column, separator);
}

call_result_t perform_unroll(int value, cell_t *cell, int direction_inv) {
int value_a;
	if (cell->value == 0 && cell->operations[direction_inv] == OPERATION_DIVIDE) {
		cell->value = value;
	}
	if (direction_inv == DIRECTION_WEST) {
		perform_operation(cell, DIRECTION_WEST, cell-1, DIRECTION_EAST, &value_a);
	}
	else if (direction_inv == DIRECTION_NORTH) {
		perform_operation(cell, DIRECTION_NORTH, cell-columns_n, DIRECTION_SOUTH, &value_a);
	}
	else if (direction_inv == DIRECTION_EAST) {
		perform_operation(cell, DIRECTION_EAST, cell+1, DIRECTION_WEST, &value_a);
	}
	else if (direction_inv == DIRECTION_SOUTH) {
		perform_operation(cell, DIRECTION_SOUTH, cell+columns_n, DIRECTION_NORTH, &value_a);
	}
	return CALL_RESULT_SUCCESS;
}

int test_direction(cell_t *cell_a, int direction_a, cell_t *cell_b) {
	switch (cell_a->operations[direction_a]) {
	case OPERATION_ADD:
		if (cell_b->value < 0) {
			if (cell_a->value < INT_MIN-cell_b->value) {
				fprintf(stderr, "%d %c %d would overflow\n", cell_a->value, OPERATION_ADD, cell_b->value);
				fflush(stderr);
				return 0;
			}
			return 1;
		}
		else {
			if (cell_a->value > INT_MAX-cell_b->value) {
				fprintf(stderr, "%d %c %d would overflow\n", cell_a->value, OPERATION_ADD, cell_b->value);
				fflush(stderr);
				return 0;
			}
			return 1;
		}
	case OPERATION_MULTIPLY:
		if (cell_b->value < 0) {
			if (cell_a->value < 0) {
				if (cell_a->value < INT_MAX/cell_b->value) {
					fprintf(stderr, "%d %c %d would overflow\n", cell_a->value, OPERATION_MULTIPLY, cell_b->value);
					fflush(stderr);
					return 0;
				}
				return 1;
			}
			else {
				if (cell_b->value != -1 && cell_a->value > INT_MIN/cell_b->value) {
					fprintf(stderr, "%d %c %d would overflow\n", cell_a->value, OPERATION_MULTIPLY, cell_b->value);
					fflush(stderr);
					return 0;
				}
				return 1;
			}
		}
		else if (cell_b->value > 0) {
			if (cell_a->value < 0) {
				if (cell_a->value < INT_MIN/cell_b->value) {
					fprintf(stderr, "%d %c %d would overflow\n", cell_a->value, OPERATION_MULTIPLY, cell_b->value);
					fflush(stderr);
					return 0;
				}
				return 1;
			}
			else {
				if (cell_a->value > INT_MAX/cell_b->value) {
					fprintf(stderr, "%d %c %d would overflow\n", cell_a->value, OPERATION_MULTIPLY, cell_b->value);
					fflush(stderr);
					return 0;
				}
				return 1;
			}
		}
		else {
			return 1;
		}
	case OPERATION_SUBTRACT:
		if (cell_b->value < 0) {
			if (cell_a->value > INT_MAX+cell_b->value) {
				fprintf(stderr, "%d %c %d would overflow\n", cell_a->value, OPERATION_SUBTRACT, cell_b->value);
				fflush(stderr);
				return 0;
			}
			return 1;
		}
		else {
			if (cell_a->value < INT_MIN+cell_b->value) {
				fprintf(stderr, "%d %c %d would overflow\n", cell_a->value, OPERATION_SUBTRACT, cell_b->value);
				fflush(stderr);
				return 0;
			}
			return 1;
		}
	case OPERATION_DIVIDE:
		if (cell_a->value == INT_MIN && cell_b->value == -1) {
			fprintf(stderr, "%d %c %d would overflow\n", cell_a->value, OPERATION_DIVIDE, cell_b->value);
			fflush(stderr);
			return 0;
		}
		return cell_b->value != 0 && cell_a->value%cell_b->value == 0;
	default:
		return 0;
	}
}

void perform_operation(cell_t *cell_a, int direction_a, cell_t *cell_b, int direction_b, int *value_a) {
int operation = cell_a->operations[direction_a];
	switch (operation) {
	case OPERATION_ADD:
		*value_a = cell_a->value;
		cell_a->value = cell_b->value;
		cell_b->value += *value_a;
		cell_a->operations[direction_a] = OPERATION_SUBTRACT;
		cell_b->operations[direction_b] = OPERATION_SUBTRACT;
		break;
	case OPERATION_MULTIPLY:
		*value_a = cell_a->value;
		cell_a->value = cell_b->value;
		cell_b->value *= *value_a;
		cell_a->operations[direction_a] = OPERATION_DIVIDE;
		cell_b->operations[direction_b] = OPERATION_DIVIDE;
		break;
	case OPERATION_SUBTRACT:
		*value_a = cell_a->value;
		cell_a->value = cell_b->value;
		cell_b->value = *value_a-cell_b->value;
		cell_a->operations[direction_a] = OPERATION_ADD;
		cell_b->operations[direction_b] = OPERATION_ADD;
		break;
	case OPERATION_DIVIDE:
		*value_a = cell_a->value;
		cell_a->value = cell_b->value;
		if (cell_b->value == 0) {
			cell_b->value = *value_a;
		}
		else {
			cell_b->value = *value_a/cell_b->value;
		}
		cell_a->operations[direction_a] = OPERATION_MULTIPLY;
		cell_b->operations[direction_b] = OPERATION_MULTIPLY;
		break;
	default:
		break;
	}
}

void free_data(void) {
	if (choices_max > 0) {
		free(choices);
	}
	if (calls_max > 0) {
		free(calls);
	}
	free(cells);
}

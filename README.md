# LabyGamePrograms

Generator and Solver for the Laby game: http://laby.thr.pm/.

## Generator

Generates a random grid in a format compatible with the solver.

Takes 11 arguments on standard input:
```
4 6             -- Grid size (number of rows/columns, excluding operator cells)
-10 10          -- Minimum/Maximum cell value
24 12 18 6      -- Add/Multiply/Subtract/Divide operator frequencies
2 3             -- Starting cell (0-based index)
100             -- Goal value
```
## Solver

Solves a grid, the algorithm is an iterative depth first search performed until a solution is found (solution found is guaranteed to be one among the shortest ones).

Sample grid format (read on standard input):
```
4 4             -- Grid size (number of rows/columns, excluding operator cells)
6 * 3 - 4 * 9   -- Grid data
-   /   /   -
1 - 4 / 8 + 5
-   /   -   +
3 - 1 * 7 * 2
+   *   +   /
13 + 6 + 2 + 5
3 0             -- Starting cell (0-based index)
8               -- Goal value
```
The laby_files folder contains grids from the real game (laby_level*.txt), and some test grids.

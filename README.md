# Puzzles #

## Nonograms ##

To solve a nonogram, you must fill a grid with black squares.
Each row and column has a list of numbers which denotes the number of black squares required, and how they are to be grouped.
Try it at http://www.puzzle-nonograms.com

The crux of this program is the row_iterator class, which, given a vector describing the groupings of black squares, iterates over every possible way to arrange them. For example, if a row of length 5 must contain a blocks of length 2 and 1, there are three ways to achieve this:

* X X _ X _
* X X _ _ X
* _ X X _ X

Notice that in all three cases, the second cell is black.
The nonogram::solve() function iterates over each row and column and collects every arrangement which satisfies the hints and is consistent with the already known cells.
If every arrangement agrees on the identity of a particular cell, its value is recorded.
This procedure is repeated until the puzzle is solved.
This method assumes that the puzzle has a unque solution, otherwise this procedure will repeat indefinitely.

I include an example input file, taken from the above website (25x25, ID: 706,720).
The program solves the puzzle in 10 passes.
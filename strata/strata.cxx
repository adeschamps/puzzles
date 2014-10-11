#include <iostream>
#include <sstream>
#include <stack>
#include <vector>

int main ()
{
	uint n_rows, n_cols;
	std::cin >> n_rows >> n_cols;
	std::vector<std::vector<char>> grid (n_rows, std::vector<char>(n_cols, '.'));
	std::stack<std::string> solution;
	std::vector<bool> solved_row (n_rows, false);
	std::vector<bool> solved_col (n_cols, false);

	for (uint row = 0; row != n_rows; ++row)
		for (uint col = 0; col != n_cols; ++col)
			std::cin >> grid[row][col];

	while (solution.size() != n_rows + n_cols)
	{
		// Iterate over rows
		for (uint row = 0; row != n_rows; ++row)
			if (solved_row[row] == false)
			{
				char c = '.';
				// If the row contains at most one colour
				// As a side effect, c will contain the value of that colour
				// I realize how lispy this is.
				if ([&] () {
						for (uint col = 0; col != n_cols; ++col)
							if (c == '.')
								c = grid[row][col];
							else
								if (grid[row][col] != '.' && grid[row][col] != c) return false;
						return true;
					}())
				{
					std::stringstream ss;
					ss << "Row " << row << ": " << c;
					solution.push(ss.str());
					for (uint col = 0; col != n_cols; ++col)
						grid[row][col] = '.';
					solved_row[row] = true;
				}
			}

		// Same thing over columns
		for (uint col = 0; col != n_cols; ++col)
			if (solved_col[col] == false)
			{
				char c = '.';
				if ([&] () {
						for (uint row = 0; row != n_rows; ++row)
							if (c == '.')
								c = grid[row][col];
							else
								if (grid[row][col] != '.' && grid[row][col] != c) return false;
						return true;
					}())
				{
					std::stringstream ss;
					ss << "Col " << col << ": " << c;
					solution.push(ss.str());
					for (uint row = 0; row != n_rows; ++row)
						grid[row][col] = '.';
					solved_col[col] = true;
				}
			}
	}

	while (solution.empty() == false)
	{
		std::cout << solution.top() << '\n';
		solution.pop();
	}
}

#include <algorithm>
#include <iostream>
#include <set>
#include <map>
#include <tuple>
#include <vector>

std::string colour (char c)
{
  switch (c)
    {
    case 'w': return "\e[38;5;15m";  // White
    case 'r': return "\e[38;5;9m";   // Red
    case 'o': return "\e[38;5;202m"; // Orange
    case 'y': return "\e[38;5;11m";  // Yellow
    case 'l': return "\e[38;5;10m";  // Lime
    case 'g': return "\e[38;5;22m";  // Green
    case 'c': return "\e[38;5;14m";  // Cyan
    case 'b': return "\e[38;5;20m";  // Blue
    case 'p': return "\e[38;5;13m";  // Pink
    default:  return "\e[90m";       // Default
    }
}

using cell = std::tuple<int,int,int>;

cell rotate (cell c, int r)
{
  if (r % 6 == 0) return c;

  return rotate (cell(-std::get<1>(c),
		      -std::get<2>(c),
		      -std::get<0>(c)),
		 r-1);
}

cell reflect (cell c, int r)
{
  if (r % 2 == 0) return c;

  cell c2 (std::get<1>(c),
	   std::get<0>(c),
	   std::get<2>(c));

  return c2;
}

cell operator + (cell c1, cell c2)
{
  return cell (std::get<0>(c1) + std::get<0>(c2),
	       std::get<1>(c1) + std::get<1>(c2),
	       std::get<2>(c1) + std::get<2>(c2));
}

struct board
{
  std::map<cell, int> cells;

  int rows, columns;
};

struct piece
{
  std::set<cell> cells;
  char colour;
};

std::istream & operator >> (std::istream & is, board & b)
{
  is >> b.rows >> b.columns;

  int index = 0;

  for (int r=0; r != b.rows; ++r)
    for (int c=0; c != b.columns; ++c)
      {
	char token;
	is >> token;
	if (token == '.')
	  {
	    int x = c - (r + r%2) / 2;
	    int z = r;
	    int y = -x -z;
	    b.cells.emplace(cell(x,y,z), index);
	    ++index;
	  }
      }

  return is;
}

std::ostream & operator << (std::ostream & os, board const & b)
{
  for (int r=0; r != b.rows; ++r)
    {
      if (r % 2 == 0)
	os << "  ";
      for (int c=0; c != b.columns; ++c)
	{
	  int x = c - (r + r%2) / 2;
	  int z = r;
	  int y = -x -z;
	  cell loc (x,y,z);
	  if (b.cells.count(loc) == 1)
	    os << colour(b.cells.at(loc)) << "(X) " << colour('\0');
	  else
	    os << "    ";
	}
      os << '\n';
    }
  return os;
}

void print_board (std::ostream & os, board const & b, std::vector<char> const & colour_vector)
{
  for (int r=0; r != b.rows; ++r)
    {
      if (r % 2 == 0)
	os << "  ";
      for (int c=0; c != b.columns; ++c)
	{
	  int x = c - (r + r%2) / 2;
	  int z = r;
	  int y = -x -z;
	  cell loc (x,y,z);
	  if (b.cells.count(loc) == 1)
	    os << colour(colour_vector.at(b.cells.at(loc))) << "(X) " << colour('\0');
	  else
	    os << "    ";
	}
      os << '\n';
    }
}

bool solve (std::vector< std::map<std::vector<bool>, bool> > & piece_coverage, std::vector<bool> & board, int index)
{
  // std::cerr << "Trying to solve at index: " << index << '\n';
  // std::cerr << "board: ";
  // for (bool b : board)
  //   std::cerr << b;
  // std::cerr << '\n';

  if (std::all_of(board.begin(), board.end(), [](bool b){ return b; }))
    return true;

  // Select the piece at index
  std::map<std::vector<bool>, bool> & piece = piece_coverage[index];

  // Loop through each position for this piece
  for (auto & positioning : piece)
    {
      // If the position fits ...
      if (std::equal(board.begin(), board.end(),
		     positioning.first.begin(),
		     [](bool a, bool b){ return (a && b) == false; }))
	{
	  // ... then set its bits to true
	  positioning.second = true;
	  for (uint i=0; i != positioning.first.size(); ++i)
	    board[i] = board[i] || positioning.first[i];

	  // Recurse
	  if (solve(piece_coverage, board, index+1))
	    return true;

	  // If solve(...) did not return true, then remove the bits
	  // that were set in the previous loop
	  positioning.second = false;
	  for (uint i=0; i != positioning.first.size(); ++i)
	    board[i] = board[i] != positioning.first[i];
	}
    }

  // If none of the possible positions lead to a solution, then return false.
  return false;
}


int main ()
{
  board b;
  std::cin >> b;
  // std::cout << b;

  // std::vector<piece> pieces;
  std::map<char, piece> pieces;

  // pieces['r'].cells.insert(cell( 0,  0,  0));

  pieces['o'].cells.insert(cell( 0,  0,  0));
  pieces['o'].cells.insert(cell( 1, -1,  0));
  pieces['o'].cells.insert(cell( 1, -2,  1));
  pieces['o'].cells.insert(cell( 0, -2,  2));
  pieces['o'].cells.insert(cell(-1, -1,  2));

  pieces['y'].cells.insert(cell( 0,  0,  0));
  pieces['y'].cells.insert(cell( 1, -1,  0));
  pieces['y'].cells.insert(cell( 2, -2,  0));
  pieces['y'].cells.insert(cell( 0, -1,  1));
  pieces['y'].cells.insert(cell( 2, -3,  1));

  pieces['l'].cells.insert(cell( 0,  0,  0));
  pieces['l'].cells.insert(cell( 1, -1,  0));
  pieces['l'].cells.insert(cell( 2, -2,  0));
  pieces['l'].cells.insert(cell( 0, -1,  1));

  pieces['g'].cells.insert(cell( 0,  0,  0));
  pieces['g'].cells.insert(cell( 1, -1,  0));
  pieces['g'].cells.insert(cell( 2, -2,  0));
  pieces['g'].cells.insert(cell( 0, -1,  1));
  pieces['g'].cells.insert(cell( 0, -2,  2));

  pieces['c'].cells.insert(cell( 0,  0,  0));
  pieces['c'].cells.insert(cell( 1, -1,  0));
  pieces['c'].cells.insert(cell( 1, -2,  1));
  pieces['c'].cells.insert(cell( 0, -2,  2));
  pieces['c'].cells.insert(cell(-1, -2,  3));

  pieces['b'].cells.insert(cell( 0,  0,  0));
  pieces['b'].cells.insert(cell( 1, -1,  0));
  pieces['b'].cells.insert(cell( 2, -2,  0));
  pieces['b'].cells.insert(cell( 2, -3,  1));

  pieces['p'].cells.insert(cell( 0,  0,  0));
  pieces['p'].cells.insert(cell( 1, -1,  0));
  pieces['p'].cells.insert(cell( 1, -2,  1));
  pieces['p'].cells.insert(cell( 0, -2,  2));

  std::vector< std::map<std::vector<bool>, bool> > piece_coverage (pieces.size());
  std::vector<char> colour_index (pieces.size());

  int index = 0;
  // For each piece
  for (std::pair<char, piece> const & p : pieces)
    {
      // For each cell on the board
      for (std::pair<cell,int> const & translation : b.cells)
	// For each rotation ...
	for (int rot=0; rot != 6; ++rot)
	  // ... and each reflection
	  for (int ref=0; ref != 2; ++ref)
	    {
	      // std::cerr << "rot: " << rot << "\tref: " << ref << '\n';
	      // Transform the piece
	      std::set<cell> transformed_piece;
	      for (cell const & c : p.second.cells)
		transformed_piece.insert(rotate(reflect(c, ref), rot) + translation.first);

	      // If every cell of the piece can fit in the board ...
	      if (std::all_of(transformed_piece.begin(), transformed_piece.end(),
			      [&](cell const & c) { return b.cells.count(c) == 1; }))
		{
		  // Then construct a vector<bool> and insert it into the map
		  std::vector<bool> coverage (b.cells.size(), false);
		  for (cell const & c : transformed_piece)
		    coverage[b.cells.at(c)] = true;
		  piece_coverage[index].emplace(std::move(coverage), false);
		}
	    }

      colour_index[index] = p.first;
      ++index;
    }

  for (auto const & pc : piece_coverage)
    {
      std::cout << "map::size(): " << pc.size() << '\n';
    }

  std::vector<bool> bool_board (b.cells.size(), false);
  std::cout << solve(piece_coverage, bool_board, 0) << '\n';

  std::vector<char> colour_vector (b.cells.size(), '\0');
  for (uint pc = 0; pc != piece_coverage.size(); ++pc)
    for (std::pair<std::vector<bool>, bool> const & p : piece_coverage.at(pc))
      if (p.second)
	for (uint i=0; i != p.first.size(); ++i)
	  if (p.first[i])
	    colour_vector[i] = colour_index[pc];

  print_board(std::cout, b, colour_vector);
}

#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <vector>

//////////////
// generics //
//////////////

template <typename A, typename B>
std::istream & operator >> (std::istream & is, std::pair<A,B> & p)
{
  return is >> p.first >> p.second;
}

/////////////
// colours //
/////////////

std::string colour (char c)
{
  switch (c)
    {
    case 'r': return "\e[31m";
    case 'g': return "\e[32m";
    case 'y': return "\e[33m";
    case 'b': return "\e[36m";
    case 'o': return "\e[37m";
    default:  return "\e[39m";
    }
}

///////////
// board //
///////////

using location = std::pair<int,int>;

location operator + (location const & a, location const & b)
{
  return location {a.first + b.first, a.second + b.second};
}

class board
{
  std::string grid;
public:
  int height, width;

  char at (location const & l) const
  { return grid[l.first*width + l.second]; }

  char at (int h, int w) const
  { return grid[h*width + w]; }

  friend std::istream & operator >> (std::istream & is, board & b);
  // friend std::ostream & operator << (std::ostream & os, board const & b);
};

std::istream & operator >> (std::istream & is, board & b)
{
  is >> b.height >> b.width;
  for (int i=0; i != b.height * b.width; ++i)
    {
      char c;
      is >> c;
      b.grid.push_back(c);
    }
  return is;
}

std::ostream & operator << (std::ostream & os, board const & b)
{
  for (int h=0; h != b.height; ++h)
    {
      for (int w=0; w != b.width; ++w)
	os << b.at(h,w);
      os << '\n';
    }
  return os;
}

///////////
// other //
///////////

bool inbounds (location const & l, board const & b)
{
  return (l.first >= 0  && l.first < b.height &&
	  l.second >= 0 && l.second < b.width);
}

///////////////
// gamestate //
///////////////

using gamestate = std::map<location, char>;

std::istream & operator >> (std::istream & is, gamestate & gs)
{
  int tokens;
  is >> tokens;
  for (int t=0; t != tokens; ++t)
    {
      std::pair<location, char> token;
      is >> token;
      gs.insert(token);
    }
  return is;
}

std::ostream & operator << (std::ostream & os, gamestate const & gs)
{
  for (std::pair<location,char> const & token : gs)
    os << colour(token.second)
       << "(" << token.first.first << ", " << token.first.second << ")"
       << colour('\0') << '\n';
  return os;
}

gamestate slide (gamestate gs, board const & b, char d)
{
  location dir = [] (char d) {
    switch (d)
      {
      case 'u': return location {-1,  0};
      case 'd': return location { 1,  0};
      case 'l': return location { 0, -1};
      case 'r': return location { 0,  1};
      default:  return location { 0,  0};
      }
  } (d);

  gamestate new_gs;
  while (gs.empty() == false)
    {
      for (auto iter = gs.begin(); iter != gs.end(); )
	{
	  location new_loc = iter -> first + dir;
	  if (inbounds(new_loc, b) == false || b.at(new_loc) == 'x' || new_gs.count(new_loc) == 1)
	    {
	      new_gs.emplace(iter -> first, iter -> second);
	      iter = gs.erase(iter);
	    }
	  else if (gs.count(new_loc) == 0)
	    {
	      new_gs.emplace(new_loc, iter -> second);
	      iter = gs.erase(iter);
	    }
	  else
	    ++iter;
	}
    }

  return new_gs;
}

bool solved (gamestate const & gs, board const & b)
{
  return std::all_of (gs.begin(), gs.end(),
		      [&] (std::pair<location, char> const & piece)
		      { return b.at(piece.first) == piece.second; });
}

void print_board (std::ostream & os, board const & b, gamestate const & gs)
{
  for (int h=0; h != b.height; ++h)
    {
      for (int w=0; w != b.width; ++w)
	{
	  location l {h,w};

	  if (b.at(l) == 'x')
	    os << "[=]";
	  else
	    {
	      os << colour(b.at(l)) << (b.at(l) == '.' ? ' ' : '<')
		 << (gs.count(l) == 1 ? colour(gs.at(l)) + "O" : " ")
		 << colour(b.at(l)) << (b.at(l) == '.' ? ' ' : '>')
		 << colour('\0');
	    }
	}
      os << '\n';
    }
}

int main ()
{
  board b;
  std::cin >> b;

  gamestate gs;
  std::cin >> gs;

  std::queue<gamestate> node_queue;
  node_queue.push(gs);
  std::map<gamestate, std::pair<gamestate,char>> whence_map;
  whence_map[gs] = std::make_pair(gs, '\0');

  while (node_queue.empty() == false && solved(node_queue.front(), b) == false)
    {
      for (char d : std::string("udlr"))
	{
	  gamestate next_gs = slide (node_queue.front(), b, d);
	  if (whence_map.count(next_gs) == 0)
	    {
	      node_queue.push(next_gs);
	      whence_map.emplace(next_gs, std::make_pair(node_queue.front(), d));
	    }
	}
      node_queue.pop();
    }

  if (node_queue.empty())
    std::cout << "no solution.\n";
  else
    {
      std::cout << "a solution was found:\t";
      std::list<char> solution;
      gamestate s = node_queue.front();
      while (whence_map[s].second != '\0')
	{
	  solution.push_front(whence_map[s].second);
	  s = whence_map[s].first;
	}
      for (char c : solution)
	std::cout << c;
      std::cout << '\n';
    }

  // std::map<gamestate, int> open_nodes;
  // std::map<gamestate, int> visited_nodes;

  // open_nodes[gs] = 0;

  // for (auto const & node : open_nodes)
  //   std::cout << "distance: " << node.second << '\n'
  // 	      << node.first
  // 	      << '\n';

  // while (open_nodes.empty() == false)
  //   {
  //     std::cerr << "open_nodes.size(): " << open_nodes.size() << '\n';
  //     auto iter = open_nodes.begin();
  //     for (char d : std::string("udlr"))
  // 	{
  // 	  gamestate next_gs = slide (iter -> first, b, d);
  // 	  if (visited_nodes.count(next_gs) == 0)
  // 	    {
  // 	      std::cerr << "considering next_gs from " << d << "...\n";
  // 	      if (open_nodes.count(next_gs) == 0 || open_nodes.at(next_gs) < iter -> second)
  // 		{
  // 		  std::cerr << "modifying open_nodes...\n";
  // 		  open_nodes[next_gs] = iter -> second + 1;
  // 		}
  // 	      std::cerr << "done with " << d << '\n';
  // 	    }
  // 	}
  //     std::cerr << "visited_nodes.size(): " << visited_nodes.size() << '\n';
  //     visited_nodes.insert(*iter);
  //     std::cerr << "visited_nodes.size(): " << visited_nodes.size() << '\n';
  //     open_nodes.erase(iter);
  //     std::cerr << "open_nodes.size(): " << open_nodes.size() << '\n';
  //   }

  // for (std::pair<gamestate, int> const & pair : visited_nodes)
  //   {
  //     std::cout << "distance: " << pair.second << '\n';
  //     print_board (std::cout, b, pair.first);
  //     std::cout << '\n';
  //   }

  // print_board (std::cout, b, gs);
  // std::cout << "solved? " << (solved(gs,b) ? "yes" : "no") << '\n';
  // gs = slide (gs, b, 'd');
  // print_board (std::cout, b, gs);
  // std::cout << "solved? " << (solved(gs,b) ? "yes" : "no") << '\n';
  // gs = slide (gs, b, 'r');
  // print_board (std::cout, b, gs);
  // std::cout << "solved? " << (solved(gs,b) ? "yes" : "no") << '\n';
}

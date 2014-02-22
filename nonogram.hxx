#ifndef AD_NONOGRAM_HXX
#define AD_NONOGRAM_HXX

#include <iostream>
#include <vector>

namespace ad
{
  class nonogram
  {
    uint W, H;
    std::vector<std::vector<uint>> cols;
    std::vector<std::vector<uint>> rows;

    enum cell {open, black, white};
    std::vector< std::vector<cell> > grid;

    class row_iterator;

    std::vector<cell> work_row (std::vector<cell> const & known, std::vector<uint> const & hints);

  public:
    nonogram ()
    {}

    void solve ();

    friend std::istream & operator >> (std::istream & is, nonogram & ng);
    friend std::ostream & operator << (std::ostream & os, nonogram const & ng);
  };

  void nonogram::solve ()
  {
    bool solved;
    do
      {
	solved = true;

	// Work rows
	for (uint h=0; h < H; ++h)
	  {
	    std::vector<cell> new_row = work_row (grid[h], rows[h]);

	    for (uint i=0; i<W; ++i)
	      if (grid[h][i] == open)
		{
		  grid[h][i] = new_row[i];
		  solved = false;
		}
	  }

	// Work columns
	for (uint w=0; w < W; ++w)
	  {
	    std::vector<cell> col (H);
	    for (uint i=0; i<H; ++i)
	      col[i] = grid[i][w];

	    std::vector<cell> new_col = work_row (col, cols[w]);

	    for (uint i=0; i<H; ++i)
	      if (grid[i][w] == open)
		{
		  grid[i][w] = new_col[i];
		  solved = false;
		}
	  }
      }
    while (solved == false);
  }

  std::istream & operator >> (std::istream & is, nonogram & ng)
  {
    // Read dimensions and resize grid
    is >> ng.W >> ng.H;
    ng.grid.resize(ng.H);
    for (auto & row : ng.grid)
      row.resize(ng.W);


    // Read hints; each group of hints is terminated by a 0

    // Read vertical hints
    ng.cols.resize(ng.W);
    for (auto & hints : ng.cols)
      {
	hints.resize (0);
	uint h;
	is >> h;
	while (h != 0)
	  {
	    hints.push_back(h);
	    is >> h;
	  }
      }

    // Read horizontal hints
    ng.rows.resize(ng.H);
    for (auto & hints : ng.rows)
      {
	hints.resize(0);
	uint h;
	is >> h;
	while (h != 0)
	  {
	    hints.push_back(h);
	    is >> h;
	  }
      }
    return is;
  }

  std::ostream & operator << (std::ostream & os, nonogram const & ng)
  {
    for (uint h=0; h < ng.H; ++h)
      {
	for (uint w=0; w < ng.W; ++w)
	  switch (ng.grid[h][w])
	    {
	    case nonogram::open:
	      os << ' ';
	      break;
	    case nonogram::black:
	      os << 'X';
	      break;
	    case nonogram::white:
	      os << '_';
	      break;
	    }
	os << '\n';
      }

    return os;
  }

  class nonogram::row_iterator
  {
    uint length;
    std::vector<uint> hints;
    std::vector<uint> starts;
    std::vector<uint> max_starts;
    std::vector<cell> row;
    bool row_ready = false;
    bool end = false;

    void init ()
    {
      row.resize (length);

      // Initial starting positions; all blocks are as far left as possible
      {
	uint d=0;
	for (uint hint : hints)
	  {
	    starts.push_back(d);
	    d += hint + 1;
	  }
      }

      // Determine maximum start position for each block
      {
	max_starts.resize(starts.size());
	uint d = length;
	uint i = starts.size();
	do
	  {
	    -- i;
	    d -= hints[i];
	    max_starts[i] = d;
	    d -= 1;
	  }
	while (i > 0);
      }

      // std::cerr << "max starts:\t";
      // for (uint ms : max_starts) std::cerr << ms << '\t';
      // std::cerr << '\n';
    }

    // Fills the row with white, then paints black blocks from each start position
    void gen_row ()
    {
      for (cell & c : row)
	c = white;

      // Could benefit from boost::zip_iterator
      for (uint i=0; i<starts.size(); ++i)
	{
	  for (uint d=0; d<hints[i]; ++d)
	    row[starts[i]+d] = black;
	}

      row_ready = true;
    }

  public:
    row_iterator (uint length_, std::vector<uint> const & hints_)
      : length (length_),
	hints (hints_)
    { init(); }

    row_iterator (uint length_, std::vector<uint> && hints_)
      : length (length_),
	hints (hints_)
    { init(); }

    row_iterator (row_iterator const & iter)
      : length (iter.length),
	hints (iter.hints),
	row (iter.row)
    {}

    // Generate row if necessary, then return it
    std::vector<cell> & operator * ()
    {
      if (row_ready == false) gen_row();
      return row;
    }

    row_iterator & operator ++ ()
    {
      // end condition
      if (starts.front() == max_starts.front())
	{
	  end = true;
	  return *this;
	}

      // Determine last block which starts below its maximum
      uint h;
      for (h = hints.size() - 1; starts[h] == max_starts[h]; --h);

      // ++ starts[h];
      // uint pos = starts[h] + hints[h] + 1;
      // for (uint i = h + 1; i < hints.size(); ++i)
      // 	{
      // 	  starts[i] =
      // 	}

      uint s = starts[h] + 1;
      for ( ; h < hints.size(); ++h)
	{
	  starts[h] = s;
	  s += hints[h] + 1;
	}

      row_ready = false;
      return *this;
    }

    row_iterator operator ++ (int)
    { row_iterator temp (*this); ++(*this); return temp; }

    bool operator == (row_iterator iter) const
    { return !(*this != iter); }

    bool operator != (row_iterator iter) const
    {
      if (end != iter.end)       return true;
      if (hints != iter.hints)   return true;
      if (starts != iter.starts) return true;
      return false;
    }

    bool done () const
    { return end; }
  };

  std::vector<nonogram::cell> nonogram::work_row (std::vector<cell> const & known, std::vector<uint> const & hints)
  {
    std::vector<std::vector<cell>> possible_results;

    auto consistent = [] (std::vector<cell> const & test, std::vector<cell> const & reference)
      {
	for (uint i=0; i < test.size(); ++i)
	  {
	    if (reference[i] != open)
	      if (test[i] != reference[i])
		return false;
	  }
	return true;
      };

    row_iterator iter (known.size(), hints);
    while (iter.done() == false)
      {
	if (consistent(*iter, known))
	  possible_results.push_back(*iter);
	++ iter;
      }

    std::vector<cell> result (known.size());
    for (uint i=0; i < known.size(); ++i)
      {
	cell c = possible_results.front()[i];
	for (uint r=1; c != open && r < possible_results.size(); ++r)
	  {
	    if (possible_results[r][i] != c)
	      c = open;
	  }
	result[i] = c;
      }
    return result;
  }
}

#endif

#include "nonogram.hxx"

#include <iostream>

int main ()
{
  ad::nonogram ng;

  std::cin >> ng;
  ng.solve();
  std::cout << ng;
}

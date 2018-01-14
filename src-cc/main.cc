#include "Board.h"
#include <iostream>

int main() {
  Board& b = *Board::New();
  b.PlayStone(true, 3);
  b.PlayStone(false, 1);
  b.PlayStone(true, 0);
  b.PlayStone(false, 1);
  b.PlayStone(true, 1);
  b.PlayStone(false, 2);
  b.PlayStone(true, 2);
  b.PlayStone(false, 6);
  b.PlayStone(true, 0);
  std::cout << b << '\n';

  Board *b2 = Board::New(b.Encode());
  std::cout << '\n' << *b2 << '\n';
}

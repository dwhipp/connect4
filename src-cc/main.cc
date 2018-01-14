#include "Board.h"
#include "Player.h"

#include <iostream>

int main() {
  Board* b = Board::New();

  Player* p[] = {
    Player::NewHuman("Player 1"),
    Player::NewHuman("Player 2")};

  p[0]->StartGame(b, true);
  p[1]->StartGame(b, false);

  bool player_to_move = 0;
  while (!b->ValidMoves().empty()) {
    b->PlayStone(player_to_move == 0, p[player_to_move]->GetMove());
    player_to_move = 1 - player_to_move;
  }

  std::cout << '\n' << *b << '\n';
}

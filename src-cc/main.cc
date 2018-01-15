#include "Board.h"
#include "Player.h"

#include <iostream>
#include <memory>
#include <random>

int main() {
  std::unique_ptr<Board> b = Board::New();

  Player* p[] = {
    Player::NewHuman("Human"),
    Player::NewComputer("Computer", 6, .99999, .9999, std::make_unique<std::random_device>())};

  p[0]->StartGame(b.get(), true);
  p[1]->StartGame(b.get(), false);

  bool player_to_move = 0;
  while (!b->ValidMoves().empty()) {
    std::cout << *b << '\n';
    for (int i = 1; i <= 7; i++) {
      std::cout << i << " ";
    }
    std::cout << "\n\n";
    if (b->PlayStone(player_to_move == 0, p[player_to_move]->GetMove())) {
      std::cout << "\n\n" << *p[player_to_move] << " WINS!!!\n";
      break;
    }
    player_to_move = 1 - player_to_move;
  }

  std::cout << '\n' << *b << '\n';
}

#include "Board.h"
#include "Player.h"
#include "Game.h"

#include <iostream>
#include <memory>
#include <random>

int main() {
  std::unique_ptr<Board> b = Board::New();

  Game g(
    Player::NewHuman("Human"),
    Player::NewComputer("Computer", 1, .99999, .9999, std::make_unique<std::random_device>()));

  Player* result = g.Play();

  std::cout << g << "\n\n";
  if (result == nullptr) {
    std::cout << "DRAW!\n";
  } else {
    std::cout << *result << " WINS!!!\n";
  }
}

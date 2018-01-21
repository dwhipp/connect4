#include "Board.h"
#include "Player.h"
#include "Game.h"

#include <iostream>
#include <memory>
#include <random>

int main() {
  std::unique_ptr<Board> b = Board::New();

  auto p1 = Player::NewHuman("Human");
  [[maybe_unused]] auto p2 = Player::NewBruteForce(
      "Brute Force", 5, .9999, .999, std::make_unique<std::random_device>());
  auto p3 = Player::NewMonteCarlo(
      "Monte Carlo", 1000, std::make_unique<std::random_device>());

  Game g(p1, p3);
  Player* result = g.Play();

  std::cout << g << "\n\n";
  if (result == nullptr) {
    std::cout << "DRAW!\n";
  } else {
    std::cout << *result << " WINS!!!\n";
  }
}

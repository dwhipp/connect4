#include <cmath>
#include <iostream>
#include <memory>
#include <random>

#include "Board.h"
#include "Player.h"
#include "Game.h"

int main(int argc, const char* argv[]) {
  std::unique_ptr<Board> b = Board::New();

  std::unique_ptr<Player> players[2];
  std::string player_names[2] = {"h:Human", "m:Monte Carlo"};

  if (argc == 3) {
    player_names[0] = argv[1];
    player_names[1] = argv[2];
  } else if (argc > 1) {
    std::cerr << "usage: " << argv[0] << " <player> <player>\n";
    std::cerr << "where\n";
    std::cerr << "  player is a string [hbm]:...\n";
  }

  for (int i = 0; i < 2; i++) {
    players[i] = Player::New(player_names[i]);
    if (players[i] == nullptr) {
      std::cerr << "Unable to initialize player\n";
      exit(1);
    }
  }

  Game g(players[0].get(), players[1].get());
  Player* result = g.Play();

  std::cout << g << "\n\n";
  if (result == nullptr) {
    std::cout << "DRAW!\n";
  } else {
    std::cout << *result << " WINS!!!\n";
  }
}

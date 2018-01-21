#include "Player.h"
#include <iostream>
#include <exception>
#include "Board.h"

class HumanPlayer : public Player {
  void StartGame(const Board* board, bool player_id) override {
    (void) player_id;
    board_ = board;
  }

  int GetMove() override {
    int move;
    while ((std::cout << "Player [" << *this << "]: ") &&
        (std::cin >> move)) {
      if (board_->IsValidMove(move - 1)) {
        return move - 1;
      }
      std::cout << "Invalid move, try again.\n\n";
    }

    throw std::runtime_error("No move provided");
  }

  const Board* board_;

  public:
    using Player::Player;
};

Player* Player::NewHuman(std::string_view name) {
  return new HumanPlayer(name);
}


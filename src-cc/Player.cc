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
    std::cout << *board_ << '\n';
    for (int i = 1; i <= 7; i++) {
      std::cout << i << " ";
    }
    int move;
    while ((std::cout << "\n\nPlayer [" << *this << "]: ") &&
        (std::cin >> move)) {
      if (board_->IsValidMove(move - 1)) {
        return move - 1;
      }
      std::cout << "Invalid move, try again.";
    }

    throw std::runtime_error("No move provided");
  }

  const Board* board_;

  public:
    virtual ~HumanPlayer() {}
    using Player::Player;
};

Player* Player::NewHuman(std::string_view name) {
  return new HumanPlayer(name);
}

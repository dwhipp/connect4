#include "Player.h"
#include <algorithm>
#include <iostream>
#include <exception>
#include <memory>
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
    virtual ~HumanPlayer() {}
    using Player::Player;
};

Player* Player::NewHuman(std::string_view name) {
  return new HumanPlayer(name);
}





class ComputerPlayer : public Player {

  const int kMaxLookahead = 10000000;

  void StartGame(const Board* board, bool player_id) override {
    player_id_ = player_id;
    board_ = board;
  }

  std::vector<double> EvaluateMoves(const Board* board, bool player, int depth, int *move_count) {
    (*move_count)--;
    std::vector<double> weights(board->kWidth);
    for (int move : board->ValidMoves()) {
      auto tmp = board->Clone();
      if (tmp->PlayStone(player, move)) {
        weights[move] = .99;
      } else if ((depth > 3) || (*move_count < 0)) {
        weights[move] = player == player_id_ ? 0.95 : .1;
      } else {
        std::vector<double> w =
          EvaluateMoves(tmp.get(), !player, depth + 1, move_count);
        double worst_case = *std::max_element(w.begin(), w.end());
        weights[move] = 1 - worst_case;
      }
    }
    return weights;
  }

  int GetMove() override {
    int move_count = kMaxLookahead;
    std::vector<double> weights =
      EvaluateMoves(board_, player_id_, 1, &move_count);
    for (unsigned int i = 0 ; i < weights.size(); i++) {
      std::cout << (i+1) << " = " << weights[i] << "\n";
    }
    std::discrete_distribution<int> dist(weights.begin(), weights.end());
    int selection = dist(*random_device_);
    std::cout << *this << " Plays " << selection << "\n\n";
    return selection;
  }

  const Board* board_;
  bool player_id_;
  std::unique_ptr<std::random_device> random_device_;

  public:
  ComputerPlayer(std::string_view name,
      std::unique_ptr<std::random_device> rd)
    : Player(name), random_device_(std::move(rd)) {}

  virtual ~ComputerPlayer() {}
};

Player* Player::NewComputer(std::string_view name,
    std::unique_ptr<std::random_device> rd) {
  return new ComputerPlayer(name, std::move(rd));
}

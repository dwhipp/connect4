#include "Player.h"
#include <algorithm>
#include <iostream>
#include <exception>
#include <cmath>
#include <memory>
#include <string>
#include <string_view>
#include "Board.h"

namespace {
  template<typename T1, typename T2> void EnsureValueInRange(std::string name, T2 min, T1 value, T2 max) {
    if (value > min && value < max) {
      return;
    }
    throw std::range_error(
        "Invalid Value [" + name + "=" + std::to_string(value) + "]: NOT " +
        std::to_string(min) + " < " + name + " < " + std::to_string(max));
  }
}

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

 public:
  int kMaxDepth;
  double kSharpness;
  double kDiscount;

  void StartGame(const Board* board, bool player_id) override {
    player_id_ = player_id;
    board_ = board;
  }

  std::vector<double> EvaluateMoves(const Board* board, bool player, int depth) {
    std::vector<double> weights(board->kWidth);
    for (int move : board->ValidMoves()) {
      auto tmp = board->Clone();
      if (tmp->PlayStone(player, move)) {
        weights[move] = kSharpness;
      } else if (depth <= 0) {
        weights[move] = 0.5;
      } else {
        std::vector<double> w =
          EvaluateMoves(tmp.get(), !player, depth - 1);
        double worst_case = *std::max_element(w.begin(), w.end());
        weights[move] = 1 - (worst_case * kDiscount);
      }
    }
    return weights;
  }

  int GetMove() override {
    std::vector<double> weights =
      EvaluateMoves(board_, player_id_, kMaxDepth);
    for (unsigned int i = 0 ; i < weights.size(); i++) {
      std::cout << (i+1) << " = " << weights[i] << "\n";
    }
    std::discrete_distribution<int> dist(weights.begin(), weights.end());
    int selection = dist(rand_);
    std::cout << *this << " Plays " << selection << "\n\n";
    return selection;
  }

  const Board* board_;
  bool player_id_;
  std::mt19937 rand_;

  public:
  ComputerPlayer(std::string_view name,
      int depth, double sharpness, double discount,
      std::unique_ptr<std::random_device> rd)
    : Player(name),
      kMaxDepth(depth), kSharpness(sharpness), kDiscount(discount), rand_((*rd)())
       {
        EnsureValueInRange("depth", 0, depth, 10);
        EnsureValueInRange("sharpness", 0.0, sharpness, 1.0);
        EnsureValueInRange("discount", 0.0, discount, 1.0);
      }

  virtual ~ComputerPlayer() {}
};

Player* Player::NewComputer(std::string_view name,
    int depth, double sharpness, double discount,
    std::unique_ptr<std::random_device> rd) {
  return new ComputerPlayer(name, depth, sharpness, discount, std::move(rd));
}

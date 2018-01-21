#include "Player.h"

#include <algorithm>
#include <cmath>
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "Board.h"

class MonteCarloPlayer : public Player {
  public:
    const int kNumRollouts;
    const double kExplorationParameter;

    void StartGame(const Board* board, bool player_id) override {
      board_ = board;
      player_id_ = player_id;
      tree_.clear();
    }

    struct Node {
      int visits = 0;
      bool is_terminal;
      int reward = 0;
      std::vector<uint64_t> next;
    };

    double CalculateUct(const Node& node, int parent_visits, bool player) const {
      if (node.visits == 0) {
        return 1000.0;
      }
      double exploit = node.reward;
      if (!node.is_terminal) {
        exploit /= node.visits;
      }
      if (player != player_id_) {
        exploit = 1 - exploit;
      }
      double explore = kExplorationParameter *
        std::sqrt(std::log(parent_visits) / node.visits);
      return exploit + explore;
    }

    int SelectNodeIndex(const Node& node, bool player, bool use_utc) {
      if (node.next.empty()) {
        throw std::runtime_error(
            "attempt to select next node from terminal state");
      }
      std::vector<double> uct;
      for (uint64_t key : node.next) {
        const Node& n = tree_[key];
        if (use_utc) {
          uct.push_back(CalculateUct(n, node.visits, player));
        } else {
          uct.push_back(n.visits);
        }
      }

      double highest = *std::max_element(uct.begin(), uct.end());
      for (auto& elem : uct) {
        if (elem != highest) {
          elem = 0;
        }
      }

      std::discrete_distribution<int> dist(uct.begin(), uct.end());
      return dist(rand_);
    }

    void Expand(uint64_t parent_key, bool player) {
      Node& node = tree_[parent_key];
      if (node.is_terminal) {
        return;
      }
      auto parent_board = Board::New(parent_key);
      auto valid_moves = parent_board->ValidMoves();
      if (valid_moves.empty()) {
        node.is_terminal = true;
        node.reward = 1;
        return;
      }
      for (int move : valid_moves) {
        auto child_board = parent_board->Clone();
        bool game_over = child_board->PlayStone(player, move);
        uint64_t child_key = child_board->Encode();
        if (game_over) {
          auto& n = tree_[child_key];
          n.reward = (player == player_id_) ? 2 : 0;
          n.is_terminal = true;
        }
        node.next.push_back(child_key);
      }
    }

    float Mcts(uint64_t key, bool player) {
      Node& node = tree_[key];

      if (node.visits == 0) {
        Expand(key, player);
      }

      node.visits++;

      if (node.next.empty()) {
        return node.reward;
      }

      float result = Mcts(node.next[SelectNodeIndex(node, player, true)], !player);
      node.reward += result;
      return result;
    }

    int GetMove() override {
      auto valid_moves = board_->ValidMoves();

      if (valid_moves.empty()) {
        throw std::runtime_error("no valid moves");
      }

      uint64_t root_key = board_->Encode();
      for (int i = 0; i < kNumRollouts; ++i) {
        Mcts(root_key, player_id_);
      }

      const Node& root = tree_[root_key];
      std::cout << "Root visits: " << root.visits << '\n';
      int i = 0;
      for (uint64_t child : root.next) {
        const Node& n = tree_[child];
        std::cout << "utc[" << (valid_moves[i++]+1) << "] = " << CalculateUct(n, root.visits, player_id_)
          << " - " << n.reward << "/" << n.visits << " terminal=" << n.is_terminal
          << '\n';
          int j = 0;
          for (uint64_t gt : n.next) {
            const Node& m = tree_[gt];
            std::cout << "... utc[" << (valid_moves[j++]+1) << "] = "
              << CalculateUct(m, n.visits, !player_id_)
              << " - " << m.reward << "/" << m.visits << " terminal=" << m.is_terminal
              << '\n';

          }
      }

      int move = valid_moves[SelectNodeIndex(root, player_id_, false)];
      std::cout << "\nMCTS Plays " << (move+1) << '\n';
      return move;
    }

  private:
    const Board* board_;
    bool player_id_;
    std::mt19937 rand_;

    std::map<uint64_t, Node> tree_;

  public:
    MonteCarloPlayer(std::string_view name, int num_rollouts,
        std::unique_ptr<std::random_device> rd)
      : Player(name),
        kNumRollouts(num_rollouts),
        kExplorationParameter(std::sqrt(2)),
        rand_((*rd)()) {}
};

Player* Player::NewMonteCarlo(std::string_view name, int num_rollouts,
            std::unique_ptr<std::random_device> rd) {
  return new MonteCarloPlayer(name, num_rollouts, std::move(rd));
}


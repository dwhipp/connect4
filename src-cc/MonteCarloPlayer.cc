#include "Player.h"

#include <algorithm>
#include <cmath>
#include <exception>
#include <functional>
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
      float reward = 0;
      std::vector<uint64_t> next;
    };

    struct Turn {
      MonteCarloPlayer* player_;
      uint64_t board_state_;
      bool turn_player_id_;
      bool is_opponent_;
      Node& node_;
      const Turn* parent_ = nullptr;

      Turn(MonteCarloPlayer* player) :
        player_(player),
        board_state_(player->board_->Encode()),
        turn_player_id_(player->player_id_),
        is_opponent_(false),
        node_(player_->tree_[board_state_])
      {}

      Turn(const Turn* parent, uint64_t board_state) :
        player_(parent->player_),
        board_state_(board_state),
        turn_player_id_(!parent->turn_player_id_),
        is_opponent_(!parent->is_opponent_),
        node_(player_->tree_[board_state]),
        parent_(parent)
      {}

      std::vector<Turn> NextTurns() const {
        std::vector<Turn> turns;
        for (uint64_t key : node_.next) {
          turns.emplace_back(this, key);
        }
        return turns;
      }

      double CalculateUct() const {
        if (node_.visits == 0) {
          return 1000.0;
        }
        double exploit = node_.reward;
        if (!node_.is_terminal) {
          exploit /= node_.visits;
        }
        if (!is_opponent_) {
          exploit = 1 - exploit;
        }
        double explore = player_->kExplorationParameter *
          std::sqrt(std::log(parent_->node_.visits) / node_.visits);
        return exploit + explore;
      }

      double CalculateRootScore() const {
        return node_.visits;
      }

      int SelectNodeIndex(decltype(&Turn::CalculateUct) score_fn) const {
        auto next_turns = NextTurns();
        if (next_turns.empty()) {
          throw std::runtime_error(
              "attempt to select next node from terminal state");
        }
        std::vector<double> uct;
        for (const auto& next_turn : next_turns) {
          uct.push_back(std::invoke(score_fn, next_turn));
        }

        double highest = *std::max_element(uct.begin(), uct.end());
        for (auto& elem : uct) {
          if (elem != highest) {
            elem = 0;
          }
        }

        std::discrete_distribution<int> dist(uct.begin(), uct.end());
        return dist(player_->rand_);
      }

      void Expand() {
        if (node_.is_terminal) {
          return;
        }
        auto board = Board::New(board_state_);
        auto valid_moves = board->ValidMoves();
        if (valid_moves.empty()) {
          node_.is_terminal = true;
          node_.reward = 0.5;
          return;
        }
        for (int move : valid_moves) {
          auto [is_terminal, child_key] = board->PlayHypothetical(
              turn_player_id_, move);
          if (is_terminal) {
            Turn next_turn(this, child_key);
            Node& child = next_turn.node_;
            child.reward = is_opponent_ ? 0 : 1;
            child.is_terminal = true;
          }
          node_.next.push_back(child_key);
        }
      }

      float Mcts() {
        if (node_.visits == 0) {
          Expand();
        }

        ++node_.visits;

        if (node_.is_terminal) {
          return node_.reward;
        }

        int selected = SelectNodeIndex(&Turn::CalculateUct);
        Turn next_turn = NextTurns()[selected];
        float result = node_.visits == 1 ? next_turn.RandomPlayout()
                                         : next_turn.Mcts();
        node_.reward += result;
        return result;
      }

      float RandomPlayout() {
        auto board = Board::New(board_state_);
        bool who = turn_player_id_;
        while (true) {
          auto valid_moves = board->ValidMoves();
          if (valid_moves.empty()) {
            return 0.5;
          }
          std::uniform_int_distribution<> dist(0, valid_moves.size() - 1);
          if (board->PlayStone(who, valid_moves[dist(player_->rand_)])) {
            return who == player_->player_id_ ? 1 : 0;
          }
          who = !who;
        }
      }
    };

    int GetMove() override {
      auto valid_moves = board_->ValidMoves();

      if (valid_moves.empty()) {
        throw std::runtime_error("no valid moves");
      }

      Turn turn(this);

      for (int i = 0; i < kNumRollouts; ++i) {
        turn.Mcts();
      }

      const Node& root = turn.node_;
      std::cout << "Root visits: " << root.visits
        << ", tree_size=" << tree_.size()
        << '\n';
      int i = 0;
      for (const auto& next_turn : turn.NextTurns()) {
        std::cout << "utc[" << (valid_moves[i++]+1) << "] = "
          << next_turn.CalculateUct()
          << " - " << next_turn.node_.reward << "/" << next_turn.node_.visits
          << " terminal=" << next_turn.node_.is_terminal
          << '\n';

          int j = 0;
          for (const auto& next_next_turn : next_turn.NextTurns()) {
            const Node& m = next_next_turn.node_;
            std::cout << "... utc[" << (valid_moves[j++]+1) << "] = "
              << next_next_turn.CalculateUct()
              << " - " << m.reward << "/" << m.visits
              << " terminal=" << m.is_terminal
              << '\n';
          }
      }

      int move = valid_moves[turn.SelectNodeIndex(&Turn::CalculateRootScore)];
      std::cout << "\n" << name() << " plays " << (move+1) << '\n';
      return move;
    }

  private:
    const Board* board_;
    bool player_id_;
    std::mt19937 rand_;

    std::map<uint64_t, Node> tree_;

  public:
    MonteCarloPlayer(
        std::string_view name, int num_rollouts, double exploration,
        std::unique_ptr<std::random_device> rd)
      : Player(name),
        kNumRollouts(num_rollouts),
        kExplorationParameter(exploration),
        rand_((*rd)()) {}
};

std::unique_ptr<Player> Player::NewMonteCarlo(std::string_view name,
    int num_rollouts, double exploration,
    std::unique_ptr<std::random_device> rd) {
  return std::unique_ptr<Player>{new MonteCarloPlayer(name, num_rollouts, exploration, std::move(rd))};
}


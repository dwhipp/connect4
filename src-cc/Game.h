#include <memory>
#include <iostream>
#include "Board.h"
#include "Player.h"

class Game {
  public:
    Game(Player* p1, Player* p2);
    void Dump(std::ostream& os) const;
    Player* Play() const;

  private:
    Player* players_[2];
    std::unique_ptr<Board> board_;
};

inline std::ostream& operator << (std::ostream& os, const Game& game) {
  game.Dump(os);
  return os;
}


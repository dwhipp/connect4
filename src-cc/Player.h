#ifndef Player_h_
#define Player_h_

#include <iostream>
#include <string>
#include <string_view>

class Board;

class Player {
  public:
    static Player *NewHuman(std::string_view name);

    std::string_view name() const { return name_; }

    virtual ~Player() {}
    virtual void StartGame(const Board* board, bool player_id) = 0;
    virtual int GetMove() = 0;

    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;

  protected:
    Player(std::string_view name) : name_(name) {}

  private:
    std::string name_;
};

inline std::ostream& operator<<(std::ostream& os, const Player& player) {
  return os << player.name();
}

#endif
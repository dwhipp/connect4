#ifndef _BOARD_H
#define _BOARD_H

#include <cinttypes>
#include <iostream>

class Board {
  public:
    const int kWidth = 7;
    const int kHeight = 6;

    static Board *New(uint64_t position = 0);
    virtual ~Board() {}

    virtual bool PlayStone(bool player, int column) = 0;

    virtual uint64_t Encode() const = 0;
    virtual void Decode(uint64_t position) = 0;
    virtual void Dump(std::ostream& os) const = 0;

    Board(const Board&) = delete;
    Board& operator=(const Board&) = delete;
  protected:
    Board() {}
};

inline std::ostream& operator << (std::ostream& os, const Board& board) {
  board.Dump(os);
  return os;
}

#endif

#include "Board.h"

#include <vector>
#include <exception>

namespace {
  int intlog2(int value) {
    int result = 0;
    while (value > 0) {
      value /= 2;
      result ++;
    }
    return result;
  }
}

class BoardImpl : public Board {
  const int kHeightBits = intlog2(kHeight);
  const uint64_t kHeightMask = (1ull << kHeightBits) - 1;
  std::vector<std::vector<bool>> cells;

  bool PlayStone(bool player, int column) override {
    if (cells.size() <= column) {
      throw std::out_of_range("column index too large");
    }
    if (cells[column].size() > kHeight) {
      throw std::overflow_error("column is full");
    }
    cells[column].push_back(player);
    return true;
  }

  char get_cell(int row, int column) const {
    if ((column >= cells.size()) ||
        (row >= cells[column].size())) {
      return '.';
    }
    return cells[column][row] ? 'X' : 'O';
  }

  void Dump(std::ostream& os) const override {
    for (int row = kHeight - 1; row >= 0; --row) {
      for (int col = 0; col < kWidth; ++col) {
        if (col != 0) {
          os << ' ';
        }
        os << get_cell(row, col);
      }
      if (row != 0) {
        os << '\n';
      }
    }
  }

  uint64_t Encode() const override {
    uint64_t value = 0;
    for (int col = kWidth - 1; col >= 0; --col) {
      const int height = cells[col].size();
      for (int row = 0; row < height; ++row) {
        value <<= 1;
        value |= cells[col][row] ? 1u: 0u;
      }
      value <<= kHeightBits;
      value |= height;
    }
    return value;
  }

  void Decode(uint64_t position) override {
    for (int col = 0; col < kWidth; ++col) {
      int height = position & kHeightMask;
      position >>= kHeightBits;
      cells[col].resize(height);
      for (int row = height - 1; row >= 0; --row) {
        cells[col][row] = position & 1;
        position >>= 1;
      }
    }
  }

  public:
  BoardImpl() : cells(kWidth) {}
  /*explict*/ BoardImpl(uint64_t position) : BoardImpl() {
    Decode(position);
  }
};

Board *Board::New(uint64_t position) {
  return new BoardImpl(position);
}

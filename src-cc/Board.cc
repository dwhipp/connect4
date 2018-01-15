#include "Board.h"

#include <utility>
#include <optional>
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

class Slice {
  public:
    Slice(std::vector<std::vector<bool>>& data, std::pair<int, int> center) :
      data_(data), center_(center) {}

    int SpanLength(std::pair<int, int> direction) const {
      int length = 1;
      bool root = ValueAt(center_);
      for (int i = 1; IsSame(root, i, direction); ++i) ++length;
      for (int i = -1; IsSame(root, i, direction); --i) ++length;
      return length;
    }

  private:
    bool ValueAt(std::pair<int, int> point) const {
      return data_[point.second][point.first];
    }

    bool IsSame(bool value, int index, std::pair<int,int> direction) const {
      int col = center_.second + index * direction.second;
      int row = center_.first + index * direction.first;
      if (row < 0 || col < 0 ||
          col >= (int)data_.size() || row >= (int)data_[col].size()) {
        return false;
      }
      return data_[col][row] == value;
    }

  private:
    const std::vector<std::vector<bool>>& data_;
    std::pair<int, int> center_;

};

class BoardImpl : public Board {
  const int kHeightBits = intlog2(kHeight);
  const uint64_t kHeightMask = (1ull << kHeightBits) - 1;
  std::vector<std::vector<bool>> cells;
  std::optional<std::pair<int,int>> last_move_;

  bool IsValidMove(int column) const override {
    if ((column < 0) || (column >= (int)cells.size())) {
      return false;
    }
    return (int)cells[column].size() < kHeight;
  }

  std::vector<int> ValidMoves() const override {
    std::vector<int> result;
    for (int i = 0; i < kWidth; ++i) {
      if (IsValidMove(i)) {
        result.push_back(i);
      }
    }
    return result;
  }

  bool PlayStone(bool player, int column) override {
    if (column < 0) {
      throw std::out_of_range("column index too small");
    }
    if (column >= (int)cells.size()) {
      throw std::out_of_range("column index too large");
    }
    int height = cells[column].size();
    if (height > kHeight) {
      throw std::overflow_error("column is full");
    }
    cells[column].push_back(player);

    std::pair<int, int> pos = {height, column};
    last_move_ = pos;
    Slice s(cells, pos);
    for (const auto& direction :
        std::vector<std::pair<int, int>>{{0, 1}, {1, 0}, {1, -1}, {1, 1}}) {
      if (s.SpanLength(direction) >= 4) {
        return true;
      }
    }

    return false;
  }

  char get_cell(int row, int col) const {
    if ((col >= (int)cells.size()) ||
        (row >= (int)cells[col].size())) {
      return '.';
    }
    if (last_move_ == std::pair<int, int>{row, col}) {
      return cells[col][row] ? 'X' : 'O';
    } else {
      return cells[col][row] ? 'x' : 'o';
    }
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

std::unique_ptr<Board> Board::New(uint64_t position) {
  return std::unique_ptr<Board>(new BoardImpl(position));
}

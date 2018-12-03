#include "Board.h"

#include <array>
#include <utility>
#include <optional>
#include <vector>
#include <exception>

namespace {

class BoardData {
  static constexpr uint64_t kSimdOne = 0x0101010101010101ull;

  class Column {
    unsigned char value_;

   public:
    int size() const {
      return 31 - __builtin_clz(value_);
    }

    bool operator[](int row_index) const {
      auto mask = 0x1u << row_index;
      return (value_ & mask) != 0;
    }

    void push_back(bool value) {
      auto mask = 0x1u << size();
      if (!value) value_ ^= mask;
      value_ |= mask << 1;
    }

    void pop_back() {
      auto mask = 0x1u << size();
      value_ ^= mask;
      value_ |= mask >> 1;
    }

    unsigned char bitmap(bool player) const {
      auto mask = 0x1u << size();
      unsigned char result = value_ ^ mask;
      if (!player) result ^= mask - 1;
      return result;
    }
  };

  union Magic {
    std::array<Column, 8> columns;
    uint64_t encoded;
    Magic(uint64_t encoded_position = 0) :
        encoded(encoded_position != 0 ? encoded_position : kSimdOne) {}
    static_assert(sizeof(columns) == sizeof(encoded));
  } board_state_;

 public:
  BoardData() : board_state_() {}
  explicit BoardData(uint64_t encoded_position) : board_state_(encoded_position) {}

  uint64_t Encode() const { return board_state_.encoded; }
  void Decode(uint64_t position) { board_state_.encoded = position; }

  int size() const { return Board::kWidth; }
  const Column& operator[](int column_index) const { return board_state_.columns[column_index]; }
  Column& operator[](int column_index) { return board_state_.columns[column_index]; }

  auto GetBitmap(bool player) const {
    std::array<unsigned char, Board::kWidth> result;
    for (int i = 0 ; i < Board::kWidth; i++) {
      result[i] = board_state_.columns[i].bitmap(player);
    }
    return result;
  }
};

class Slice {
  private:
    const int col_;
    const int row_;
    const unsigned char mask_;
    const std::array<unsigned char, Board::kWidth> bitmap_;

  public:
    Slice(BoardData data, std::pair<int, int> center) :
      col_(center.first),
      row_(center.second), 
      mask_(1u << row_),
      bitmap_(data.GetBitmap(data[col_][row_])) {}

    int SpanLength(std::pair<int, int> direction) const {
      auto BoardBits = [&](int offset) -> unsigned char {
        int index = col_ + offset * direction.first;
        if (index < 0 || index >= Board::kWidth) return 0;
        return bitmap_[index];
      };
      auto MaskBits = [&](int offset) -> unsigned char {
        return mask_ >> offset * direction.second;
      };
      int length = 1;
      for (int i = 1; BoardBits(i) & MaskBits(i); ++i) ++length;
      for (int i = -1; BoardBits(i) & MaskBits(i); --i) ++length;
      return length;
    }
};

class BoardImpl : public Board {
  BoardData cells;
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

    std::pair pos = {column, height};
    last_move_ = pos;
    Slice s(cells, pos);
    for (const auto& direction : std::initializer_list<std::pair<int, int>>{
        {0, 1}, {1, 0}, {1, -1}, {1, 1}}) {
      if (s.SpanLength(direction) >= 4) {
        return true;
      }
    }

    return false;
  }

  std::pair<bool, uint64_t> PlayHypothetical(
      bool player, int column) override {
    bool result = PlayStone(player, column);
    uint64_t encoded = Encode();
    cells[column].pop_back();
    return {result, encoded};
  }

  char get_cell(int row, int col) const {
    if ((col >= (int)cells.size()) ||
        (row >= (int)cells[col].size())) {
      return '.';
    }
    if (last_move_ == std::pair{col, row}) {
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
    return cells.Encode();
  }

  void Decode(uint64_t position) override {
    cells.Decode(position);
  }

 public:
  BoardImpl() : cells() {}
  /*explict*/ BoardImpl(uint64_t position) : cells(position) {}
};

}

std::unique_ptr<Board> Board::New(uint64_t position) {
  return std::unique_ptr<Board>(new BoardImpl(position));
}

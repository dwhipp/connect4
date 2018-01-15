#include "Game.h"

Game::Game(Player* p1, Player* p2) :
  players_{p1, p2}, board_(Board::New()) {
}

void Game::Dump(std::ostream& os) const {
  os << *board_ << '\n';
  for (int i = 1; i <= 7; i++) {
    os << i << " ";
  }
}

Player* Game::Play() const {
  int player_to_move = 0;

  players_[0]->StartGame(board_.get(), true);
  players_[1]->StartGame(board_.get(), false);

  while (!board_->ValidMoves().empty()) {
    Player *player = players_[player_to_move];
    std::cout << *this << "\n\n";
    if (board_->PlayStone(player_to_move == 0, player->GetMove())) {
      return player;
    }
    player_to_move = 1 - player_to_move;
  }
  return nullptr;
}


connect4 : Board.o HumanPlayer.o BruteForcePlayer.o Game.o main.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -o $@ $^

CXXFLAGS := --std=c++17 -Wall -Werror -pedantic

Board.o: Board.h
HumanPlayer.o: Player.h Board.h
BruteForcePlayer.o: Player.h Board.h
Game.o: Game.h Player.h Board.h
main.o: Game.h Player.h Board.h

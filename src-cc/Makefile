connect4 : Board.o Player.o HumanPlayer.o BruteForcePlayer.o MonteCarloPlayer.o Game.o main.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -o $@ $^

clean:
	rm connect4 Board.o HumanPlayer.o BruteForcePlayer.o MonteCarloPlayer.o Game.o main.o

CXXFLAGS := --std=c++17 -O3 -Wall -Werror -pedantic

Board.o: Board.h
Player.o: Player.h
HumanPlayer.o: Player.h Board.h
BruteForcePlayer.o: Player.h Board.h
MonteCarloPlayer.o: Player.h Board.h
Game.o: Game.h Player.h Board.h
main.o: Game.h Player.h Board.h

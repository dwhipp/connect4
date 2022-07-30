OBJS := Board.o Player.o HumanPlayer.o BruteForcePlayer.o MonteCarloPlayer.o Game.o

connect4 : $(OBJS) main.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -o $@ $^

clean:
	rm connect4 Board.o HumanPlayer.o BruteForcePlayer.o MonteCarloPlayer.o Game.o main.o

CXXFLAGS := --std=c++20 -g -Wall -Werror -pedantic

connect4.so: $(OBJS)
	$(CXX) $(LDFLAGS) -shared -o $@ $^
  

Board.o: Board.h
Player.o: Player.h
HumanPlayer.o: Player.h Board.h
BruteForcePlayer.o: Player.h Board.h
MonteCarloPlayer.o: Player.h Board.h
Game.o: Game.h Player.h Board.h
main.o: Game.h Player.h Board.h

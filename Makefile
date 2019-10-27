all: compile

compile:
	@g++ -Wall main.cpp -o game -lncurses
	@gnome-terminal -- ./game
clean:
	@rm game
	@clear
all: normal pipewithgui interactive

normal:
	g++ -std=c++11 CompetitiveSnake.cpp -o bin/CompetitiveSnake -lsfml-graphics -lsfml-window -lsfml-system

pipewithgui:
	g++ -std=c++11 CompetitiveSnake.cpp -o bin/CompetitiveSnakePipeWithGui -DPIPEWITHGUI -DGUI -lsfml-graphics -lsfml-window -lsfml-system

interactive:
	g++ -std=c++11 CompetitiveSnake.cpp -o bin/CompetitiveSnakeInteractive -DINTERACTIVE -DGUI -lsfml-graphics -lsfml-window -lsfml-system

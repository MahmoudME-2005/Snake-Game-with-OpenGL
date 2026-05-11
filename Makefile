build/SnakeGame.exe: obj/main.o obj/Fruit.o obj/Snake.o obj/Wall.o obj/Game.o obj/Renderer.o
	g++ -g obj/main.o obj/Fruit.o obj/Snake.o obj/Wall.o obj/Renderer.o obj/Game.o -o build/SnakeGame.exe -lfreeglut -lopengl32 -lglu32 -lglew32

obj/Renderer.o: src/Renderer/Renderer.cpp src/Renderer/Renderer.hpp
	g++ -g -c src/Renderer/Renderer.cpp -o obj/Renderer.o

obj/Game.o: src/Game/Game.cpp src/Game/Game.hpp
	g++ -g -c src/Game/Game.cpp -o obj/Game.o

obj/main.o: src/main.cpp
	g++ -g -c src/main.cpp -o obj/main.o

obj/Snake.o: src/Snake/Snake.cpp src/Snake/Snake.hpp
	g++ -g -c src/Snake/Snake.cpp -o obj/Snake.o

obj/Wall.o: src/Wall/Wall.cpp src/Wall/Wall.hpp
	g++ -g -c src/Wall/Wall.cpp -o obj/Wall.o

obj/Fruit.o: src/Fruit/Fruit.cpp src/Fruit/Fruit.hpp
	g++ -g -c src/Fruit/Fruit.cpp -o obj/Fruit.o

run: build/SnakeGame.exe
	build/SnakeGame.exe

clean:
	del /q build\*.exe obj\*.o
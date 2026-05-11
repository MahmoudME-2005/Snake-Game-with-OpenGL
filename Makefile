build/SnakeGame.exe: obj/main.o obj/Fruit.o obj/Snake.o obj/Wall.o
	g++ -g obj/main.o obj/Fruit.o obj/Snake.o obj/Wall.o -o build/SnakeGame.exe

obj/main.o: src/main.cpp
	g++ -g -c src/main.cpp -o obj/main.o

obj/Snake.o: src/Snake.cpp
	g++ -g -c src/Snake.cpp -o obj/Snake.o

obj/Wall.o: src/Wall.cpp
	g++ -g -c src/Wall.cpp -o obj/Wall.o

obj/Fruit.o: src/Fruit.cpp
	g++ -g -c src/Fruit.cpp -o obj/Fruit.o

run: build/SnakeGame.exe
	build/SnakeGame.exe

clean:
	del /q build/*.exe obj/*.o
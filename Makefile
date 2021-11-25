all:
	g++ -o bin/main src/*.cpp -lmingw32
	bin\main test.aenc
all:test.cpp scene.cpp scene.h
	g++ -g -c scene.cpp -o scene.o
	g++ -g -c test.cpp -o test.o
	g++ -g test.o scene.o -o test

clean:
	rm test *.o

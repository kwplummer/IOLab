CXX=g++
CFLAGS=-O2 -Wall
GTEST_FLAGS=/usr/lib/libgtest.a /usr/lib/libgtest_main.a -lpthread

ALL: program tests

program: main.o iomanager.o filesystem53.o
	$(CXX) -o lab1 main.o iomanager.o filesystem53.o $(CFLAGS)

tests: tests.o iomanager.o filesystem53.o
	$(CXX) -o tests tests.o iomanager.o filesystem53.o $(GTEST_FLAGS) $(CFLAGS)

filesystem53.o: filesystem53.cpp
	$(CXX) -c -o filesystem53.o filesystem53.cpp $(CFLAGS)

iomanager.o: iomanager.cpp
	$(CXX) -c -o iomanager.o iomanager.cpp $(CFLAGS)

tests.o: tests.cpp
	$(CXX) -c -o tests.o tests.cpp $(CFLAGS)

main.o: main.cpp
	$(CXX) -c -o main.o main.cpp $(CFLAGS)

.PHONY: clean

clean:
	rm -v *.o

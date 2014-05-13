CXX=g++
CFLAGS=-O2 -Wall

ALL: program 

program: main.o iomanager.o filesystem53.o presentationshell.o
	$(CXX) -o lab1 main.o iomanager.o filesystem53.o presentationshell.o $(CFLAGS)

presentationshell.o: PresentationShell.cpp
	$(CXX) -c -o presentationshell.o PresentationShell.cpp $(CFLAGS)

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

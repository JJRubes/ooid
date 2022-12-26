connectWires.out: connectWires.cpp Pin.o
	g++ connectWires.cpp Pin.o -Wall -o connectWires.out

findBoxes.out: findBoxes.cpp BoxFinder.cpp BoxFinder.hpp
	g++ findBoxes.cpp BoxFinder.cpp -Wall -o findBoxes.out

breakDownBox.out: breakDownBox.cpp BoxFinder.o CallBox.o DefinitionBox.o Pin.o UnparsedBox.o
	g++ breakDownBox.cpp BoxFinder.o CallBox.o DefinitionBox.o Pin.o UnparsedBox.o -Wall -o breakDownBox.out

%.o: %.cpp %.hpp
	g++ -Wall -c -o $@ $<

clean:
	rm *.o *.out

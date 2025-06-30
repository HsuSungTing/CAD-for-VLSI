# PA2 cpp compiler

.PHONY: all run clean

all: exe

exe: 110501521_PA2.o
	@g++ -std=c++11 110501521_PA2.o -o exe

run:
	@./exe $(input) $(output)

clean:
	@rm -f *.o exe

110501521_PA2.o: 110501521_PA2.cpp
	@g++ -std=c++11 -c 110501521_PA2.cpp

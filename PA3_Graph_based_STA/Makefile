CXX = g++
CXXFLAGS = -Wall -std=c++11 -Iinc

TARGET = 110501521_PA3
SRCS = 110501521_PA3.cpp src/liberty_parser.cpp src/RTL_parser.cpp
OBJS = $(SRCS:.cpp=.o)

LIBFILE = test_lib.lib

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

run: $(TARGET)
	@if [ -z "$(input)" ]; then \
		echo "Error: Please provide input using 'make run input=<input.v>'"; \
	else \
		./$(TARGET) $(input) $(LIBFILE); \
	fi

clean:
	rm -f $(TARGET) *.o src/*.o *.txt
# 編譯器與選項
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

# 檔名與資料夾
TARGET = 110501521_PA4
SRC = src/Parser.cpp
INC = -Iinc

# 所有物件檔
OBJS = $(TARGET).o $(SRC:.cpp=.o)

# 預設目標
all: $(TARGET)

# 編譯主程式與Parser
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

# 編譯主程式
$(TARGET).o: $(TARGET).cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

# 編譯 Parser.cpp
src/Parser.o: src/Parser.cpp inc/Parser.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

# 執行程式
run: $(TARGET)
	@if [ -z "$(input)" ] || [ -z "$(output)" ]; then \
		echo "請使用：make run input=xxx output=yyy"; \
	else \
		./$(TARGET) $(input) $(output); \
	fi

# 清除可執行檔與所有輸出檔
clean:
	rm -f $(TARGET) *.o src/*.o *.out *.txt *.output
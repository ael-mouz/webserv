CXX = c++
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror

TARGET = server
# SRC = ./src/server.cpp ./utils/logger.cpp ./utils/convertText.cpp
SRC = ./src/main.cpp ./src/Config.cpp
OBJS = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(TARGET)

re: fclean all

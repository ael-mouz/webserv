CXX = g++
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror
TARGET = server
BIN = obj/
SRC = utils/Utils.cpp src/main.cpp src/Config.cpp src/ServerConfig.cpp src/server.cpp src/Response.cpp
HEADERS = include/Config.hpp include/Include.hpp include/Response.hpp include/ServerConfig.hpp include/server.hpp
OBJS = $(addprefix $(BIN), $(SRC:.cpp=.o))

all: $(TARGET)

$(TARGET): $(OBJS)
	@$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

$(BIN)%.o: %.cpp
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -rf $(BIN)

fclean: clean
	@rm -f $(TARGET)

re: fclean all
CXX = c++

CXXFLAGS = -std=c++98 -Wall -Wextra -Werror #-fsanitize=address

TARGET = server

BIN = obj/

# SRC = src/server.cpp utils/logger.cpp utils/convertText.cpp

SRC = src/main.cpp src/Config.cpp src/ServerConfig.cpp utils/Utils.cpp

HEADERS = include/Config.hpp include/ServerConfig.hpp

# OBJS =$(SRC:.cpp=.o)
OBJS =$(addprefix $(BIN),$(SRC:.cpp=.o))

all: $(TARGET)

$(TARGET): $(OBJS) $(HEADERS)
	@$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

$(BIN)%.o: %.cpp
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -rf $(BIN)
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(TARGET)

re: fclean all

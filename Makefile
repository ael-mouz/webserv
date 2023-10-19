CXX = c++

CXXFLAGS = -std=c++98 -Wall -Wextra -Werror # -fsanitize=address

TARGET = server

BIN = obj/

SRC = src/server.cpp src/Response.cpp  utils/logger.cpp utils/convertText.cpp #src/Client.cpp
HEADERS = include/server.hpp include/Response.hpp

# SRC = src/main.cpp src/Config.cpp src/ServerConfig.cpp utils/Utils.cpp
# HEADERS = include/Config.hpp include/ServerConfig.hpp

# SRC = src/main.cpp src/Cgi.cpp
# HEADERS = include/Cgi.hpp

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

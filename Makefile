CXX = c++
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror -g #-fsanitize=address
TARGET = webserv
BIN = obj/

SRC =	src/Request/ChunkedEncoding.cpp \
		src/Request/Headers.cpp \
		src/Request/Body.cpp \
		src/Request/RequestLine.cpp \
		src/Request/Request.cpp \
		src/Server/Client.cpp \
		src/Server/Utils.cpp \
		src/Response/Response.cpp \
		src/Config/Config.cpp \
		src/Config/MimeTypes.cpp \
		src/Config/ServerConfig.cpp \
		src/Config/Status.cpp \
		src/Server/RunServers.cpp \
		src/main.cpp

# src/Request/main.cpp

HEADERS =	include/Request/ChunkedEncoding.hpp \
			include/Request/Headers.hpp \
			include/Request/Body.hpp \
			include/Request/RequestLine.hpp \
			include/Request/Request.hpp \
			include/Server/Client.hpp \
			include/Server/Utils.hpp \
			include/Response/Response.hpp\
			include/Config/Config.hpp \
			include/Config/MimeTypes.hpp \
			include/Config/ServerConf.hpp \
			include/Config/ServerConfig.hpp \
			include/Server/RunServers.hpp \
			include/Config/Status.hpp

OBJS = $(addprefix $(BIN), $(SRC:.cpp=.o))

all: $(TARGET)

debug: CXXFLAGS += -DDEBUG_C
debug: re

$(TARGET): $(OBJS)
	@$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

$(BIN)%.o: %.cpp $(HEADERS)
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -rf $(BIN)

fclean: clean
	@rm -f $(TARGET)

re: fclean all

exec: all
	@ ./webserv ./config/config.conf

fds:
	@ while true; do pgrep -o -x webserv | xargs -I{} lsof -p {} ; sleep 4 ;clear; done

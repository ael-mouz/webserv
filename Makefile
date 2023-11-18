CXX = c++
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror -g -fsanitize=address
TARGET = webserv
BIN = obj/

V			=	$(shell tput -Txterm setaf 5)
W			=	$(shell tput -Txterm setaf 7)
Y			=	$(shell tput -Txterm setaf 3)
G			=	$(shell tput -Txterm setaf 2)
R			=	$(shell tput -Txterm setaf 1)

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

IP_ADDRESS := $(shell ifconfig | grep inet | awk 'NR==5 {print $$2}')

FLAG =	" $(G)██$(Y)╗    $(G)██$(Y)╗$(G)███████$(Y)╗$(G)██████$(Y)╗ $(G)███████$(Y)╗$(G)███████$(Y)╗$(G)██████$(Y)╗ $(G)██$(Y)╗   $(G)██$(Y)╗\n"\
		"$(G)██$(Y)║    $(G)██$(Y)║$(G)██$(Y)╔════╝$(G)██$(Y)╔══$(G)██$(Y)╗$(G)██$(Y)╔════╝$(G)██$(Y)╔════╝$(G)██$(Y)╔══$(G)██$(Y)╗$(G)██$(Y)║   $(G)██$(Y)║\n"\
		"$(G)██$(Y)║ $(G)█$(Y)╗ $(G)██$(Y)║$(G)█████$(Y)╗  $(G)██████$(Y)╔╝$(G)███████$(Y)╗$(G)█████$(Y)╗  $(G)██████$(Y)╔╝$(G)██$(Y)║   $(G)██$(Y)║\n"\
		"$(G)██$(Y)║$(G)███$(Y)╗$(G)██$(Y)║$(G)██$(Y)╔══╝  $(G)██$(Y)╔══$(G)██$(Y)╗╚════$(G)██$(Y)║$(G)██$(Y)╔══╝  $(G)██$(Y)╔══$(G)██$(Y)╗╚$(G)██$(Y)╗ $(G)██$(Y)╔╝\n"\
		"╚$(G)███$(Y)╔$(G)███$(Y)╔╝$(G)███████$(Y)╗$(G)██████$(Y)╔╝$(G)███████$(Y)║$(G)███████$(Y)╗$(G)██$(Y)║  $(G)██$(Y)║ ╚$(G)████$(Y)╔╝\n"\
		" $(Y)╚══╝╚══╝ ╚══════╝╚═════╝ ╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝$(W) \n"\
		"		made by ($(Y)ael-mouz$(W)) and ($(Y)yettabaa$(W)) \n"\
		"		generate executable ($(V)webserv$(W))"

all: $(TARGET)

# clear;
ip:
	@echo $(IP_ADDRESS)

debug: CXXFLAGS += -DDEBUG_C
debug: re
SPINNER = ⠿ ⠷ ⠶ ⠦ ⠤ ⠴ ⠾ ⠇ ⠿ ⠷ ⠶ ⠦ ⠤ ⠴ ⠾ ⠇ ⠿

export IP_ADDRESS

$(TARGET): $(OBJS)
	@$(CXX) $(CXXFLAGS) -o $@ $(OBJS)
	@for i in $(SPINNER); do \
		printf "$(Y)$$i  $(V)Compiling:$(W) $@\r"; \
		sleep 0.1;\
	done
	@clear;
	@echo  $(FLAG)


NUM_FILES	:=	$(words $(SRC))
NUM			:=	0
RES			:=	0

$(BIN)%.o: %.cpp $(HEADERS)
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@$(eval NUM := $(shell echo $$(($(NUM) + 1))))
	@$(eval RES := $(shell echo $$(($(NUM) * 100 / $(NUM_FILES)))))
	@printf "%-3s $(Y)%-32s$(W) $(G)compiled successfully ✓$(W)\n" "$(RES)%" "$<"

clean:
	@rm -rf $(BIN)

fclean: clean
	@rm -f $(TARGET)

re: fclean all

exec: all
	@ ./webserv ./config/config.conf

fds:
	@ while true; do pgrep -o -x webserv | xargs -I{} lsof -p {} ; sleep 4 ;clear; done

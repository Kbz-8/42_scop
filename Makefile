NAME = scop

SRCS =  $(wildcard $(addsuffix /*.cpp, ./Runtime/Engine/Sources/Core))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Engine/Sources/Renderer))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Engine/Sources/Platform))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Application))

OBJ_DIR = Objects
BIN_DIR = Bin

OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))

CXX = clang++
CXXFLAGS = -std=c++17 -I Runtime/Engine/Includes -I Runtime/Application -I Runtime/Engine/ThirdParty/KVF

LDFLAGS = -lvulkan -lSDL2

DEBUG ?= false
MODE = "release"

ifeq ($(DEBUG), true)
	CXXFLAGS += -g -D DEBUG
	MODE = "debug"
endif

RM = rm -rf

$(OBJ_DIR)/%.o: %.cpp
	@printf "\e[1;32m[compiling "$(MODE)" {"$(CXX)"}...]\e[1;00m "$<"\n"
	@$(CXX) $(CXXFLAGS) $(COPTS) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJ_DIR) $(BIN_DIR) $(OBJS)
	@printf "\e[1;32m[linking   "$(MODE)" {"$(CXX)"}...]\e[1;00m "$@"\n"
	@$(CXX) -o $(BIN_DIR)/$(NAME) $(OBJS) $(LDFLAGS)
	@printf "\e[1;32m[build finished]\e[1;00m\n"

$(OBJ_DIR):
	@mkdir -p $(sort $(addprefix $(OBJ_DIR)/, $(dir $(SRCS))))

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

run:
	@$(BIN_DIR)/$(NAME)

clean:
	@$(RM) $(OBJ_DIR)

fclean: clean
	@$(RM) $(NAME) $(BIN_DIR)

re: fclean all

.PHONY: all run clean fclean re 

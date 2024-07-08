NAME = scop

SRCS += $(wildcard $(addsuffix /*.cpp, ./Application))

BIN_DIR = Bin
OBJ_DIR = Objects
RES_DIR = Resources

OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))

CXX = clang++
CXXFLAGS = -std=c++17 -I ScopEngine/Runtime/Includes -I Application -I ScopEngine/ThirdParty/KVF

LDFLAGS = -lvulkan -lSDL2 ScopEngine/Bin/engine.a

DEBUG ?= false
MODE = "release"

ifeq ($(DEBUG), true)
	CXXFLAGS += -g -D DEBUG
	MODE = "debug"
endif

JOBS = $(patsubst -j%,%,$(filter -j%,$(MAKEFLAGS)))

RM = rm -rf

$(OBJ_DIR)/%.o: %.cpp
	@printf "\e[1;32m[compiling "$(MODE)" {"$(CXX)"}...]\e[1;00m "$<"\n"
	@$(CXX) $(CXXFLAGS) $(COPTS) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJ_DIR) $(BIN_DIR) engine $(OBJS)
	@printf "\e[1;32m[linking   "$(MODE)" {"$(CXX)"}...]\e[1;00m "$@"\n"
	@$(CXX) -o $(BIN_DIR)/$(NAME) $(OBJS) $(LDFLAGS)
	@printf "\e[1;32m[build finished]\e[1;00m\n"

engine:
	@make -j$(JOBS) -C ScopEngine DEBUG=$(DEBUG)

$(OBJ_DIR):
	@mkdir -p $(sort $(addprefix $(OBJ_DIR)/, $(dir $(SRCS))))

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

run:
	@$(BIN_DIR)/$(NAME) $(RES_DIR)/knuckles.obj

clean:
	@$(RM) $(OBJ_DIR)
	@make -C ScopEngine clean

fclean: clean
	@$(RM) $(BIN_DIR)
	@make -C ScopEngine fclean

re: fclean all

.PHONY: all run clean fclean re 

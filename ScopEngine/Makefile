SRCS =  $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Core))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Renderer))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Platform))

BIN_DIR = Bin
OBJ_DIR = Objects

OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))

CXX = clang++
CXXFLAGS = -std=c++17 -I Runtime/Includes -I ThirdParty/KVF

AR = ar rc

LDFLAGS = -lvulkan -lSDL2

SHARED ?= false
DEBUG ?= false
MODE = "release"

ifeq ($(DEBUG), true)
	CXXFLAGS += -g -D DEBUG
	MODE = "debug"
endif

ifeq ($(SHARED), true)
	NAME = engine.so
else
	NAME = engine.a
endif

RM = rm -rf

$(OBJ_DIR)/%.o: %.cpp
	@printf "\e[1;32m[compiling "$(MODE)" {"$(CXX)"}...]\e[1;00m "$<"\n"
	@$(CXX) $(CXXFLAGS) $(COPTS) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJ_DIR) $(BIN_DIR) $(OBJS)
	@printf "\e[1;32m[linking   "$(MODE)" {"$(CXX)"}...]\e[1;00m "$@"\n"
ifeq ($(SHARED), true)
	@$(CXX) -o $(BIN_DIR)/$(NAME) $(OBJS) $(LDFLAGS)
else
	@$(AR) $(BIN_DIR)/$(NAME) $(OBJS)
endif
	@printf "\e[1;32m[build finished]\e[1;00m\n"

$(OBJ_DIR):
	@mkdir -p $(sort $(addprefix $(OBJ_DIR)/, $(dir $(SRCS))))

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

clean:
	@$(RM) $(OBJ_DIR)

fclean: clean
	@$(RM) $(BIN_DIR)

re: fclean all

.PHONY: all run clean fclean re 


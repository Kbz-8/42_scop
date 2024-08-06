SRCS =  $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Core))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Platform))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Graphics))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Renderer))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Renderer/Vulkan))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Renderer/Pipelines))

BIN_DIR = Bin
OBJ_DIR = Objects

OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))

CXX = clang++
CXXFLAGS = -std=c++20 -I Runtime/Includes -I Runtime/Sources -I ThirdParty/KVF -D KVF_IMPL_VK_NO_PROTOTYPES

AR = ar rc

LDFLAGS = -lSDL2

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

$(NAME): $(OBJ_DIR) $(BIN_DIR) shaders $(OBJS)
	@printf "\e[1;32m[linking   "$(MODE)" {"$(CXX)"}...]\e[1;00m "$@"\n"
ifeq ($(SHARED), true)
	@$(CXX) -o $(BIN_DIR)/$(NAME) $(OBJS) $(LDFLAGS) --shared
else
	@$(AR) $(BIN_DIR)/$(NAME) $(OBJS)
endif
	@printf "\e[1;32m[build finished]\e[1;00m\n"

shaders:
	@sh ./Assets/Scripts/CompileShaders.sh $(CURDIR)

$(OBJ_DIR):
	@mkdir -p $(sort $(addprefix $(OBJ_DIR)/, $(dir $(SRCS))))

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

clean:
	@$(RM) $(OBJ_DIR)
	@$(RM) Assets/Shaders/Build

fclean: clean
	@$(RM) $(BIN_DIR)

re: fclean all

.PHONY: all run clean fclean re 


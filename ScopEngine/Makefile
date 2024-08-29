SRCS =  $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Core))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Platform))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Graphics))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Graphics/Cameras))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Graphics/Loaders))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Renderer))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Renderer/Memory))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Renderer/Vulkan))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Renderer/Pipelines))
SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Renderer/RenderPasses))

SHADER_SRCS = $(wildcard $(addsuffix /*.nzsl, ./Assets/Shaders))

BIN_DIR = Bin
OBJ_DIR = Objects
SHADER_DIR = Assets/Shaders/Build
SHADER_MODULE_DIR = Assets/Shaders/Modules

OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))
SPVS = $(addprefix $(SHADER_DIR)/, $(SHADER_SRCS:.nzsl=.spv))

CXX = clang++
CXXFLAGS = -std=c++20 -I Runtime/Includes -I Runtime/Sources -I ThirdParty/KVF -D KVF_IMPL_VK_NO_PROTOTYPES

AR = ar rc

NZSLC = ./Assets/Vendors/nzslc.x86_64

LDFLAGS = -lSDL2

SHARED ?= false
DEBUG ?= false
MODE = "release"

ifeq ($(DEBUG), true)
	CXXFLAGS += -g -D DEBUG -D IMGUI_IMPL_VULKAN_NO_PROTOTYPES -I ThirdParty/imgui
	MODE = "debug"
	SRCS += $(wildcard $(addsuffix /*.cpp, ./Runtime/Sources/Debug))
	SRCS += $(wildcard $(addsuffix /*.cpp, ./ThirdParty/imgui))
	SRCS += $(wildcard $(addsuffix /*.cpp, ./ThirdParty/imgui/backends))
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

$(SHADER_DIR)/%.spv: %.nzsl
	@printf "\e[1;32m[compiling...]\e[1;00m "$<"\n"
	@$(NZSLC) --compile=spv $< -o $(SHADER_DIR) --optimize --module=$(SHADER_MODULE_DIR)

$(OBJ_DIR):
	@mkdir -p $(sort $(addprefix $(OBJ_DIR)/, $(dir $(SRCS))))

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(SHADER_DIR):
	@mkdir -p $(SHADER_DIR)

shaders : $(SHADER_DIR) $(SPVS)

clean-shaders:
	@$(RM) $(SHADER_DIR)

re-shaders: clean-shaders shaders

clean: clean-shaders
	@$(RM) $(OBJ_DIR)

fclean: clean
	@$(RM) $(BIN_DIR)

re: fclean all

.PHONY: all clean fclean re shaders clean-shaders re-shaders


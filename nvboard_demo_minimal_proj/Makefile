TOPNAME = top
NXDC_FILES = constr/top.nxdc
INC_PATH ?=

VERILATOR = verilator
VERILATOR_CFLAGS += -MMD --build -cc  \
				-O3 --x-assign fast --x-initial fast --noassert

BUILD_DIR = ./verilator_build
OBJ_DIR = $(BUILD_DIR)/obj_dir
BIN = $(BUILD_DIR)/$(TOPNAME)
NVBOARD_HOME = $(shell pwd)/../nvboard

default: $(BIN)

$(shell mkdir -p $(BUILD_DIR))

# constraint file
SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	NVBOARD_HOME=$(NVBOARD_HOME) python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@

# project source
VSRCS = $(shell find $(abspath ./vsrc) -name "*.v")
CSRCS = $(shell find $(abspath ./csrc) -name "*.c" -or -name "*.cc" -or -name "*.cpp")
CSRCS += $(SRC_AUTO_BIND)



# rules for NVBoard
include $(NVBOARD_HOME)/scripts/nvboard.mk

# rules for verilator
INCFLAGS = $(addprefix -I, $(INC_PATH))
CFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOPNAME)\""
LDFLAGS += -lSDL2 -lSDL2_image

$(BIN): $(VSRCS) $(CSRCS) $(NVBOARD_ARCHIVE)
	@rm -rf $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_CFLAGS) \
		--top-module $(TOPNAME) $^ \
		$(addprefix -CFLAGS , $(CFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))

$(BIN)_debug: $(VSRCS) $(CSRCS) $(NVBOARD_ARCHIVE)
	@rm -rf $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_CFLAGS) \
		--top-module $(TOPNAME) $^ \
		$(addprefix -CFLAGS , $(CFLAGS) -DCLK_DEBUG_MODE) $(addprefix -LDFLAGS , $(LDFLAGS)) \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN)_debug)

all: default

build: $(BIN)

build_debug: $(BIN)_debug

run: $(BIN)
	@ NVBOARD_HOME=$(NVBOARD_HOME) $^

run_debug: $(BIN)_debug
	@ NVBOARD_HOME=$(NVBOARD_HOME) $^

# 由于CLion自带的终端无法使用快捷键Ctrl+Z和Ctrl+C发送信号，所以拉起konsole
# 使用VSCode开发可以无视，因为VSCode自带终端可以使用快捷键Ctrl+Z和Ctrl+C发送信号

run_debug_external_terminal: $(BIN)_debug
	@ NVBOARD_HOME=$(NVBOARD_HOME) konsole -e $(abspath $(BIN)_debug)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: default all clean run

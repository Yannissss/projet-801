# Paramètrage
TARGET_EXEC ?= gauss-seidel
BUILD_DIR   ?= ./build
SRC_DIRS    ?= ./src

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.cu -or -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS  := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

ifeq ($(CC), g++)
CC      ?= g++
CFLAGS  += -Wall -O3
LDFLAGS += -fopenmp -lstdc++
else
CC      ?= clang++
CFLAGS  += -Wall -O3 --gcc-toolchain=/apps/2021/gcc/10.2/
LDFLAGS += -fopenmp -lstdc++
endif

CFLAGS  += $(INC_FLAGS) -MMD -MP
CFLAGS  += $(shell pkg-config --cflags opencv4)
LDFLAGS += $(shell pkg-config --libs opencv4)

# Rules
all: $(TARGET_EXEC)

$(TARGET_EXEC): $(OBJS)
		$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
		@$(MKDIR_P) $(dir $@)
		$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
		$(RM) -r $(BUILD_DIR)
		$(RM) $(TARGET_EXEC)

-include $(DEPS)

MKDIR_P ?= mkdir -p

CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++20 -Iinc -Ithirdparty
CXXLIBS := -lvulkan -lglfw
CSHADER := glslc

SRC_DIR := src
SHADER_DIR := shaders
ifeq ($(DEBUG), 1)
	BUILD_DIR := build/debug
	CXXFLAGS += -g -Og
else
	BUILD_DIR := build/release
	CXXFLAGS += -O2
endif

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/$(SRC_DIR)/%.o, $(SRCS))
DEPS := $(OBJS:.o=.d)
VERT_SHADERS := $(wildcard $(SHADER_DIR)/*.vert)
FRAG_SHADERS := $(wildcard $(SHADER_DIR)/*.frag)
SPIRVS := $(patsubst $(SHADER_DIR)/%.vert, $(BUILD_DIR)/%_vert.spv, $(VERT_SHADERS)) $(patsubst $(SHADER_DIR)/%.frag, $(BUILD_DIR)/%_frag.spv, $(FRAG_SHADERS))
TARGET := demo-collection

all: $(SPIRVS) $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(CXXLIBS) -o $(BUILD_DIR)/$@ $^

$(BUILD_DIR)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/%_vert.spv: $(SHADER_DIR)/%.vert
	@mkdir -p $(dir $@)
	$(CSHADER) $< -o $@
	
$(BUILD_DIR)/%_frag.spv: $(SHADER_DIR)/%.frag
	@mkdir -p $(dir $@)
	$(CSHADER) $< -o $@

-include $(DEPS)

clean:
	rm -rf $(BUILD_DIR)

Makefile: ;

.PHONY: all clean


CC ?= gcc
TARGET ?= pulse_dash
BUILD_DIR ?= build
SRC := .C/pulse_dash.c

PKG_CONFIG ?= pkg-config
RAYLIB_CFLAGS := $(shell $(PKG_CONFIG) --cflags raylib 2>/dev/null)
RAYLIB_LIBS := $(shell $(PKG_CONFIG) --libs raylib 2>/dev/null)

CFLAGS ?= -std=c99 -Wall -Wextra -Wpedantic
CPPFLAGS ?= -I.H
LDLIBS ?= $(if $(RAYLIB_LIBS),$(RAYLIB_LIBS),-lraylib -lGL -lm -lpthread -ldl -lrt -lX11)

.PHONY: all run clean

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(SRC) .H/*.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(RAYLIB_CFLAGS) $(CFLAGS) $(SRC) -o $@ $(LDLIBS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: all
	./$(BUILD_DIR)/$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

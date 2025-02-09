# Compiler and linker
C = gcc
CFLAGS = -g
LDFLAGS = -lraylib -lcglm -lm
# Project-specific settings
TARGET = main
SRCS = $(wildcard src/*.c)  # Add your source files here
HEAD = $(wildcard src/*.h)  # Add your source files here
OBJS = $(SRCS:src/%.c=build/%.o)

# Build target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(C) -o $@ $(OBJS) $(LDFLAGS)

# Compile source files to object files
build/%.o: src/%.c $(HEAD)
	mkdir -p build
	$(C) -c $(CFLAGS) $< -o $@

run: $(TARGET)
	alacritty --hold --command ./$(TARGET)

# Clean up build files
clean:
	rm -f $(TARGET) $(OBJS)
	rm -rf build

.PHONY: all clean run

TARGET := my_aws_s3

BUILD_DIR := ./build
SRC_DIRS := ./src

SRCS := $(shell find $(SRC_DIRS) -name '*.cpp')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

CC := mpic++
CFLAGS := -pedantic -Werror -Wall -Wextra -g
CFLAGS=
#LDFLAGS := -fsanitize=address
LDFLAGS=

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

run: $(TARGET)
	mpiexec -hostfile hostfile -n 5 $(TARGET)

.PHONY: clean

clean:
	rm -r $(BUILD_DIR)
	rm $(TARGET)
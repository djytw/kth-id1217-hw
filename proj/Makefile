CC := gcc
LIBS := -lpthread -lm
CFLAGS := -Wall -g -O3
SRCS := $(wildcard *.c)
TARGET := $(SRCS:%.c=build/%)

ifdef DEBUG
CFLAGS += -DDEBUG
endif

all:$(TARGET)

build/%:%.c
	@mkdir -p build
	@printf "\e[0;36m$(CC) $(CFLAGS) -o $@ $^ $(LIBS)\e[0m\n"
	-@$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	@rm -rf build

CC := gcc
LIBS := -lpthread -lm
CFLAGS := -Wall -g
SRCS := $(wildcard *.c)
TARGET := $(SRCS:%.c=build/%)

ifdef TEST
CFLAGS += -DTEST
endif

all:$(TARGET)

build/%:%.c
	@mkdir -p build
	@printf "\e[0;36m$(CC) $(CFLAGS) -o $@ $^ $(LIBS)\e[0m\n"
	-@$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	@rm -rf build

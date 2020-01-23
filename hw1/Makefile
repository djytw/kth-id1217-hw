CC := gcc
LIBS := -lpthread -lm
CFLAGS := -Wall -g
SRCS := $(wildcard *.c)
TARGET := ./build/$(SRCS:%.c=%)

ifdef DEBUG
CFLAGS += -DDEBUG
endif

$(TARGET):$(SRCS)
	@mkdir -p build
	@printf "\e[0;46m$(CC) $(CFLAGS) -o $@ $^ $(LIBS)\e[0m\n"
	@$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	@echo 

clean:
	@rm -rf build

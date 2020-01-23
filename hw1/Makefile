CC := gcc
LIBS := -lpthread -lm
CFLAGS := -Wall -g
SRCS := $(wildcard *.c)
TARGET := ./build/$(SRCS:%.c=%)


all: $(TARGET)
	@mkdir -p build

$(TARGET):$(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) 

clean:
	@rm -rf build

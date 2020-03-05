CC := mpicc
SRCS := $(wildcard *.c)
TARGET := $(SRCS:%.c=build/%)

all:$(TARGET)

build/%:%.c
	@mkdir -p build
	@printf "\e[0;36m$(CC) -o $@ $^\e[0m\n"
	-@$(CC) -o $@ $^

clean:
	@rm -rf build

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

test:
	@echo "\e[0;42mTesting h1a\e[0m"
	@make testh1a
	@echo "\e[0;42mTesting h1b\e[0m"
	@make testh1b
	@echo "\e[0;42mTesting h3\e[0m"
	@make testh3
	@echo "\e[0;42mTesting h5\e[0m"
	@make testh5
	@echo "\e[0;42mTesting h7\e[0m"
	@make testh7

testh1a:
	@echo > ./logs/h1a.log
	@for i in $$(seq 1 3 24);do echo h1a  $$i Threads, Matrix 10000x10000; printf "\n$$i Threads, Matrix 10000x10000\n" >> ./logs/h1a.log;./build/h1a 10000 $$i >> ./logs/h1a.log;done

testh1b:
	@echo > ./logs/h1b.log
	@for i in $$(seq 1 3 24);do echo h1b  $$i Threads, Matrix 10000x10000; printf "\n$$i Threads, Matrix 10000x10000\n" >> ./logs/h1b.log;./build/h1b 10000 $$i >> ./logs/h1b.log;done

testh3:
	@echo > ./logs/h3.log
	@for i in $$(seq 1 3 24);do echo h3  $$i Threads, 1M Slices; printf "\n$$i Threads, 1M Slices\n" >> ./logs/h3.log;./build/h3 1000000 $$i >> ./logs/h3.log;done

testh5:
	@./build/h5 tests/h5f1.txt tests/h5f2.txt > ./logs/h5.log

testh6:
	@./build/h6 > ./logs/h6.log

testh7:
	@./build/h7 > ./logs/h7.log
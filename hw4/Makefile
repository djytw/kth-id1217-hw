CC := javac
SRCS := $(wildcard *.java)
TARGET := $(SRCS:%.java=%/)

all:$(TARGET)

%/:%.java
	@mkdir -p $@
	@printf "\e[0;36m$(CC) -g -d $@ $^\e[0m\n"
	-@$(CC) -d $@/ $^
clean:
	@rm -rf $(TARGET)

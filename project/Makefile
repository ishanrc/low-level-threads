TARGET = bin/main

SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=obj/%.o)

default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*

$(TARGET): $(OBJ)
	gcc -o $@ $?

$(OBJ): obj/%.o: src/%.c
	gcc -c $< -o $@ -Iinc -g


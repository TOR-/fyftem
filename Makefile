BIN=fyf
OBJ=entry.o

all: $(OBJ)
	ld -o $(BIN) $(OBJ)

%.o: %.asm
	nasm -g -felf64 $^ -o $@

clean:
	rm -rf $(OBJ) $(BIN)

OBJ_DIR = ./build
SERVER = ./server
CLIENT = ./client

all: $(OBJ_DIR) server client

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

server:
	cd $(SERVER) && make

client:
	cd $(CLIENT) && make

clean:
	rm $(OBJ_DIR)/*.o
	rm $(SERVER)/$(OBJ_DIR)/*.o
	rm $(CLIENT)/$(OBJ_DIR)/*.o

.PHONY: all clean server client


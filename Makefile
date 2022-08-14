SERVER_NAME = server
CLIENT_NAME = client
CC = g++
SERVER_DIR = server
CLIENT_DIR = client
INCLUDE_DIR = include
LIB_DIR = lib
OBJ_DIR = build

CFLAGS = -g -std=c++11 -I$(LIB_DIR) -I$(INCLUDE_DIR)

DEPS = $(wildcard $(LIB_DIR)/*.tpp) $(wildcard $(INCLUDE_DIR)/*.h)
_OBJ = $(wildcard $(LIB)/*.cpp)
OBJ  = $(patsubst $(LIB)/%.cpp,$(OBJ_DIR)/%.o,$(_OBJ))

_SERVER_OBJ = $(wildcard $(SERVER_DIR)/*.cpp)
SERVER_OBJ = $(patsubst $(SERVER_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(_SERVER_OBJ))

_CLIENT_OBJ = $(wildcard $(CLIENT_DIR)/*.cpp)
CLIENT_OBJ = $(patsubst $(CLIENT_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(_CLIENT_OBJ))

all: $(PROJECT_NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS)
	$(CC) -c $(CFLAGS) $< -o $@

$(SERVER_DIR): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ_DIR)/*.o -o $@

clean:
	rm $(OBJ_DIR)/*.o

.PHONY: all clean


PROJECT_NAME = knn
CC = g++
SRC_DIR = src
INCLUDE_DIR = include
LIB_DIR = lib
OBJ_DIR = build

CFLAGS = -g -I$(LIB_DIR) -I$(INCLUDE_DIR)

DEPS = $(wildcard $(LIB_DIR)/*.tpp) $(wildcard $(INCLUDE_DIR)/*.h)
_OBJ = $(wildcard $(SRC_DIR)/*.cpp)
OBJ  = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(_OBJ))

all: $(PROJECT_NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS)
	$(CC) -c $(CFLAGS) $< -o $@

$(PROJECT_NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ_DIR)/*.o -o $@

clean:
	rm $(OBJ_DIR)/*.o

.PHONY: all clean


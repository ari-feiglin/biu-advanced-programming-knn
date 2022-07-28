PROJECT_NAME = knn
CC = g++
SRC_DIR = ./src ./lib
INCLUDE_DIR = ./include
BUILD_DIR = ./build

CFLAGS = -std=c++11 -Wall $(patsubst %,-I%,$(INCLUDE_DIR))

DEPS = $(wildcard $(INCLUDE_DIR)/*,h)
__OBJ = $(patsubst %,%/*.cpp,$(SRC_DIR))
_OBJ = $(wildcard $(__OBJ))
OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(_OBJ))

all: $(BUILD_DIR) $(PROJECT_NAME)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	$(CC) -c -g $(CFLAGS) $< -o $@

$(PROJECT_NAME): $(OBJ)
	$(CC) -g $(CFLAGS) $^ -o $@

clean:
	rm $(BUILD_DIR)/*.o

.PHONY: all clean


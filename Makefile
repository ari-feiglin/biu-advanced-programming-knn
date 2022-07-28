PROJECT_NAME = knn
CC = g++
SRC_DIR = ./src
L_DIR = ./lib
INCLUDE_DIR = ./include
BUILD_DIR = ./build

CFLAGS = -std=c++11 -Wall $(patsubst %,-I%,$(INCLUDE_DIR))

DEPS = $(wildcard $(INCLUDE_DIR)/*.h)

_SOBJ = $(wildcard $(SRC_DIR)/*.cpp)
SOBJ = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(_SOBJ))

_LOBJ = $(wildcard $(L_DIR)/*.cpp)
LOBJ = $(patsubst $(LIB_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(_LOBJ))

#all: $(PROJECT_NAME)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	@mkdir $(BUILD_DIR)
	$(CC) -c -g $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(LIB_DIR)/%.c $(DEPS)
	$(CC) -c -g $(CFLAGS) $< -o $@


$(PROJECT_NAME): $(SOBJ) $(LOBG)
	$(CC) -g $(CFLAGS) $^ -o $@

clean:
	rm $(BUILD_DIR)/*.o

.PHONY: all clean


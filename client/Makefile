PROJECT_NAME = ../knnclient
CC = g++
INCLUDE_DIR = include ../include
SRC_DIR = ./src
LIB_DIR = ../lib
OBJ_DIR = ./build
LIB_OBJ_DIR = ../build

CFLAGS := -g -std=c++11 $(patsubst %,-I%,$(INCLUDE_DIR)) -I$(LIB_DIR)

DEPS := $(wildcard $(LIB_DIR)/*.tpp) $(wildcard $(patsubst %,%/*.h,$(INCLUDE_DIR)))

LIB_SRC := $(wildcard $(LIB_DIR)/*.cpp) 
SRC_SRC := $(wildcard $(SRC_DIR)/*.cpp)
SRC := $(LIB_SRC) $(SRC_SRC)

#OBJ := $(patsubst $(LIB_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(wildcard $(LIB_DIR)/*.cpp)) $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/*.cpp))
OBJ := $(patsubst $(LIB_DIR)/%.cpp,$(LIB_OBJ_DIR)/%.o,$(LIB_SRC)) $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_SRC))

all: $(OBJ_DIR) $(PROJECT_NAME)

debug::
	@echo "DEPS: $(DEPS)"
	@echo "SRC: $(SRC)"
	@echo "OBJ: $(OBJ)"

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS)
	$(CC) -c $(CFLAGS) $< -o $@

$(LIB_OBJ_DIR)/%.o: $(LIB_DIR)/%.cpp $(DEPS)
	$(CC) -c $(CFLAGS) $< -o $@

$(PROJECT_NAME): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

clean: cleanobj cleanlib

cleanobj:
	rm $(OBJ_DIR)/*.o

cleanlib:
	rm $(LIB_OBJ_DIR)/*.o

.PHONY: all clean

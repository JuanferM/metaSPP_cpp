# BIN_DIR specifies the binaries directory
BIN_DIR := bin
# SRC_DIR specifies the source directory
SRC_DIR := src
# OBJ_DIR specifies the object directory
OBJ_DIR := obj
# INC_DIR specifies the include directories
INC_DIR := include

# EXE specifies the name of the executable
EXE := $(BIN_DIR)/DM1
# SRC specifies which files to compile as part of the project
SRC := $(wildcard $(SRC_DIR)/*.cpp)
# OBJ specifies the object files
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
# INC specifies the include files
INC := $(foreach d, $(INC_DIR), -I$d)

# CC specifies which compiler we're using
CC 			:= g++
# COMPILER_FLAGS specifies the additional compilation options we're using
CXXFLAGS	:= -Wall -fpic -std=c++20
# Linker flag
LDFLAGS		:=
# LDFLAGS specifies the libraries we're linking against
LDLIBS		:= -lglpk -lm
# -I is a preprocessor flag, not a compiler flag
CPPFLAGS	:=

.PHONY: all clean

# This is the target that compiles our executable
all: $(EXE)

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) $(INC) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $(INC) -c $< -o $@

$(BIN_DIR):
	mkdir -p $@

$(OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(EXE) $(OBJ_DIR)/*

-include $(OBJ:.o=.d)

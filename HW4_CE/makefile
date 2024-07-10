TARGET = CE_E24096530
SRC_DIR = ./src
INC_DIR = ./include
# Please place your .cpp and .h file under "src" and "include" directory, respectively.
# INC_FLUTE = -I./flute-3.1 
LIB_FLUTE = -L./flute-3.1 -lflute

# -I look for .h files
# -L look for .a/.so files

SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(patsubst %.cpp, %.o, $(SRC))
DEP = $(patsubst %.cpp, %.d, $(SRC))
CC = g++
CFLAG = -std=c++11 -Wall -g -O3
${TARGET} : ${OBJ}
	${CC} ${CFLAG} $^ $(LIB_FLUTE) -o $@

# $^ all dependencies
# $< first dependency
# $@ target

-include ${DEP}

$(SRC_DIR)/%.o : $(SRC_DIR)/%.cpp
	${CC} ${CFLAG} -MMD -c -I $(INC_DIR)  $< -o $@
.PHONY : clean
clean:
	-rm ${OBJ} $(TARGET) ${DEP}
.PHONY: all dbg clean

TARGET1 = teln_srv
TARGET2 = teln_clnt

#CFLAGS = -Wall -g

CC = g++

SRC_DIR = ./src/
OBJ_DIR = ./obj/
BIN_DIR = ./build/

INCL_LIBS = $(OBJ_DIR)cross_socket.o $(OBJ_DIR)cross_socket_common.o $(OBJ_DIR)cross_socket_conn.o

INCL_CLNT = $(OBJ_DIR)cross_socket_clnt.o

INCL_SRV = $(OBJ_DIR)cross_socket_srv.o

SRC = $(wildcard $(SRC_DIR)*.cpp)
OBJ = $(patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR)%.o, $(SRC))
#BIN = $(patsubst $(SRC_DIR)%.cpp, $(BIN_DIR)%, $(SRC))
BIN := $(BIN_DIR)$(TARGET1) $(BIN_DIR)$(TARGET2)

all : if_folders_not_exist $(OBJ) $(BIN)

clean :
		rm -rf $(BIN_DIR)*  $(OBJ_DIR)*.o

if_folders_not_exist :
	if [ ! -d "$(OBJ_DIR)" ]; \
		then mkdir $(OBJ_DIR); \
	fi

	if [ ! -d "$(BIN_DIR)" ]; \
		then mkdir $(BIN_DIR); \
	fi

			
$(OBJ) : $(OBJ_DIR)%.o : $(SRC_DIR)%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

#$(BIN_DIR)% : $(OBJ_DIR)%.o 
#		$(CC) $< $(INCL_LIBS) -o $@

$(BIN_DIR)$(TARGET1) : $(OBJ)
		$(CC) $(OBJ_DIR)$(TARGET1).o $(INCL_LIBS) $(INCL_SRV) -o $@

$(BIN_DIR)$(TARGET2) : $(OBJ)
		$(CC) $(OBJ_DIR)$(TARGET2).o $(INCL_LIBS) $(INCL_CLNT) -o $@
		
dbg : 
	$(CC) -g $(SRC_DIR)$(TARGET1).cpp $(INCL_LIBS) $(INCL_SRV)
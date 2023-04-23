.PHONY: all dbg clean

TARGET11 = cs_srv
TARGET12 = cs_clnt

TARGET21 = cs_srv.exe
TARGET22 = cs_clnt.exe

CFLAGS = -Wall -g

CC = g++

CC_mingw = /usr/bin/x86_64-w64-mingw32-g++-posix
LINKED_LIBS = -static -lwsock32
#-libgcc -static-libstdc++ -lwsock32


SRC_DIR = ./src/
OBJ_DIR = ./obj_linux/
BIN_DIR = ./build_linux/
INCL_LIBS = $(OBJ_DIR)cross_socket.o $(OBJ_DIR)cross_socket_common.o $(OBJ_DIR)cross_socket_conn.o $(OBJ_DIR)cross_socket_conn_wrapper.o
INCL_CLNT = $(OBJ_DIR)cross_socket_clnt.o $(OBJ_DIR)cross_socket_clnt_udp.o $(OBJ_DIR)cross_socket_clnt_tcp.o
INCL_SRV = $(OBJ_DIR)cross_socket_srv.o $(OBJ_DIR)cross_socket_srv_tcp.o $(OBJ_DIR)cross_socket_srv_udp.o
LIB_HEADERS_DIR = ./cross_socket_lib_h/

OBJ_DIR_WIN = ./obj_windows/
BIN_DIR_WIN = ./build_windows/
INCL_LIBS_WIN = $(OBJ_DIR_WIN)cross_socket.o $(OBJ_DIR_WIN)cross_socket_common.o $(OBJ_DIR_WIN)cross_socket_conn.o $(OBJ_DIR_WIN)cross_socket_conn_wrapper.o
INCL_CLNT_WIN = $(OBJ_DIR_WIN)cross_socket_clnt.o $(OBJ_DIR_WIN)cross_socket_clnt_udp.o $(OBJ_DIR_WIN)cross_socket_clnt_tcp.o
INCL_SRV_WIN = $(OBJ_DIR_WIN)cross_socket_srv.o $(OBJ_DIR_WIN)cross_socket_srv_tcp.o $(OBJ_DIR_WIN)cross_socket_srv_udp.o

SRC = $(wildcard $(SRC_DIR)*.cpp)
HEADERS = $(wildcard $(SRC_DIR)*.h)
OBJ = $(patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR)%.o, $(SRC))
OBJ_WIN = $(patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR_WIN)%.o, $(SRC))
#BIN = $(patsubst $(SRC_DIR)%.cpp, $(BIN_DIR)%, $(SRC))
BIN := $(BIN_DIR)$(TARGET11) $(BIN_DIR)$(TARGET12)
BIN_WIN := $(BIN_DIR_WIN)$(TARGET21) $(BIN_DIR_WIN)$(TARGET22)

all : if_folders_not_exist $(OBJ) $(BIN) 

win : if_folders_not_exist $(OBJ_WIN) $(BIN_WIN) 

clean :
	rm -rf $(BIN_DIR)*  $(OBJ_DIR)*.o $(BIN_DIR_WIN)*  $(OBJ_DIR_WIN)*.o $(LIB_HEADERS_DIR)*

if_folders_not_exist :
	if [ ! -d "$(OBJ_DIR)" ]; \
		then mkdir $(OBJ_DIR); \
	fi

	if [ ! -d "$(BIN_DIR)" ]; \
		then mkdir $(BIN_DIR); \
	fi

	if [ ! -d "$(OBJ_DIR_WIN)" ]; \
		then mkdir $(OBJ_DIR_WIN); \
	fi

	if [ ! -d "$(BIN_DIR_WIN)" ]; \
		then mkdir $(BIN_DIR_WIN); \
	fi

	if [ ! -d "$(LIB_HEADERS_DIR)" ]; \
		then mkdir $(LIB_HEADERS_DIR); \
	fi

		
$(OBJ) : $(OBJ_DIR)%.o : $(SRC_DIR)%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

#$(BIN_DIR)% : $(OBJ_DIR)%.o 
#		$(CC) $< $(INCL_LIBS) -o $@

$(BIN_DIR)$(TARGET11) : $(OBJ)
		$(CC) $(OBJ_DIR)$(TARGET11).o $(INCL_LIBS) $(INCL_SRV) -o $@

$(BIN_DIR)$(TARGET12) : $(OBJ)
		$(CC) $(OBJ_DIR)$(TARGET12).o $(INCL_LIBS) $(INCL_CLNT) -o $@
		
dbg : 
	$(CC) -g $(SRC_DIR)$(TARGET11).cpp $(INCL_LIBS) $(INCL_SRV)

dbg_c : 
	$(CC) -g $(SRC_DIR)$(TARGET12).cpp $(INCL_LIBS) $(INCL_CLNT)

#win
$(OBJ_WIN) : $(OBJ_DIR_WIN)%.o : $(SRC_DIR)%.cpp
	$(CC_mingw) -D_WIN64  $(CFLAGS) -c $< -o $@

$(BIN_DIR_WIN)$(TARGET21) : $(OBJ_WIN)
		$(CC_mingw) -D_WIN64 $(OBJ_DIR_WIN)$(TARGET11).o $(INCL_LIBS_WIN) $(INCL_SRV_WIN) -o $@ $(LINKED_LIBS)


$(BIN_DIR_WIN)$(TARGET22) : $(OBJ_WIN)
		$(CC_mingw) -D_WIN64 $(OBJ_DIR_WIN)$(TARGET12).o $(INCL_LIBS_WIN) $(INCL_CLNT_WIN) -o $@ $(LINKED_LIBS)


nix_lib_all : $(OBJ)
		ar rvs $(LIB_HEADERS_DIR)lib_cross_socket1.a $(INCL_LIBS) $(INCL_SRV) $(INCL_CLNT) 
		cp -r $(HEADERS) $(LIB_HEADERS_DIR)
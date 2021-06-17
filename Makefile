###############################################################################
#	DESCRIPTION: 
#		Makefile for ftp_upload
CC = gcc
CXX = g++ -std=c++11

C_FLAGS = -Wall -g -fPIC
CFLAGS   = $(C_FLAGS)
CPPFLAGS = $(C_FLAGS)

BIN_PATH = .
PUB_LIB_HOME = /mnt/d/work/boss_lib
WBL_PATH = $(PUB_LIB_HOME)/wbl_oss/64/0.0.0

INCFLGS  = -I. -I ../boss_lib/tinyxml2/6.2.0/include -I$(WBL_PATH)/include/

LIB_LINK = -L../boss_lib/tinyxml2/6.2.0/lib/Tlinux1.2 -ltinyxml2 -L$(WBL_PATH)/lib -lwbl -lpthread

BIN = upload

OBJ = main.o ftp.o ftp_comm.o

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) -o $@ $^ $(LIB_LINK)

clean:
	rm -f $(BIN) $(OBJ)

# %.o:%.c
# 	$(CC) -c $(CFLAGS) $(INCFLGS) $<

%.o:%.cpp
	$(CXX) -c $(CPPFLAGS) $(INCFLGS) $<
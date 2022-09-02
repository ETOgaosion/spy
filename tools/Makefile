base_dir := $(readlink -f ../)
base_include_dir := $(base_dir)/include
lib_dir := $(base_dir)/lib

CC			= gcc
OBJDUMP		= objdump
GDB			= gdb

FUNC_SRC	= $(wildcard functions/*.c)
LIB_SRC		= $(wildcard ../lib/*.c)
SRC			= $(wildcard *.c) $(FUNC_SRC) $(LIB_SRC)

INCLUDE		= -Iinclude/ -I$(base_dir)/lib/ -I$(base_dir)/include/spy

all :
	$(CC) $(INCLUDE) $(SRC) -o spy
	-rm *.o functions/*.o

clean:
	-rm *.o functions/*.o spy

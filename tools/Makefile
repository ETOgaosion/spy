CC			= gcc
OBJDUMP		= objdump
GDB			= gdb

FUNC_SRC	= $(wildcard function/*.c)
SRC			= $(wildcard *.c) $(FUNC_SRC)

INCLUDE		= -Iinclude/ -I../lib/ -I../include/spy

all :
	$(CC) $(INCLUDE) $(SRC) -o spy
	rm *.o function/*.o

clean:
	rm *.o function/*.o spy

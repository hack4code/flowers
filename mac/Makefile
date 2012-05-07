
CC = cc
INC = -I"./include/"
#CFLAGS = $(INC) -Wall
CFLAGS = $(INC) -Wall -DGLFW_NO_GLU -DGLFW_INCLUDE_GL3

LIB = -L"./lib/"
LFLAGS = $(LIB) -framework OpenGL -lc -framework Cocoa -lglfw

SRC = main.c glflower.c glutil.c
OBJS = $(addprefix objs/, $(notdir $(patsubst %.c, %.o, $(SRC))))

EXE = flower

all : $(EXE)

$(EXE) : $(OBJS)
	$(CC) $(CFLAGS) $(LFLAGS) $(OBJS) -o $(EXE) 

objs/%.o : ../%.c
	$(CC) $(CFLAGS) -O0 -g3 -c -o "$@" "$<" 
	
clean:
	rm -rf $(EXE) objs/*

.PHONY: all clean

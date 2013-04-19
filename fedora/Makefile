
CC = cc
CFLAGS = -Wall

LFLAGS = -lm -lGL -lglut

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

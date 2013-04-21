
CC = cc
CFLAGS = -Wall -DGL_GLEXT_PROTOTYPES

LFLAGS = -lm -lGL -lglut

SRC = main.c glflower.c glutil.c
OBJDIR = objs
OBJS = $(addprefix $(OBJDIR)/, $(notdir $(patsubst %.c, %.o, $(SRC))))

EXE = flower

all : $(OBJDIR) $(EXE)

$(EXE) : $(OBJS)
	$(CC) $(CFLAGS) $(LFLAGS) $(OBJS) -o $(EXE) 

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o : ../%.c
	$(CC) $(CFLAGS) -O0 -g3 -c -o "$@" "$<" 
	
clean:
	rm -rf $(EXE) $(OBJDIR)/* $(OBJDIR)

.PHONY: all clean

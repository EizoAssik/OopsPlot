vpath %.c src

CFLAGS=-O2 -std=c99 -Wall
bases  := function.x lexer.x parser.x tokens.x usrfunc.x runtime.x oopsplot.x error.x memory.x optimize.x adjust.x
sources := $(bases:.x=.c)
objects := $(bases:.x=.o)

oopsplot: $(objects)
	$(CC) -lm -o oopsplot $(objects)

$(objects): $(sources)


clean:
	rm -f *~ *.o *.out
	
defualt: oopsplot

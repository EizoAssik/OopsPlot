vpath %.c src

CC=clang

bases  := function.x lexer.x parser.x tokens.x usrfunc.x runtime.x
sources := $(bases:.x=.c)
headers := $(bases:.x=.h)
objects := $(bases:.x=.o)

oopsplot: $(objects)
	$(CC) -o oopsplot $(objects)

$(objects): $(sources)


clean:
	rm -f *~ *.o *.out
	
defualt: oopsplot

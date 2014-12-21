CC=gcc
CFLAGS = -Wall -ansi --pedantic -std=gnu99 -O2 -pthread -c
LDFLAGS = -Wall -ansi --pedantic -O2 -pthread

OBJECTS = err.o list.o
ALL = serwer komisja raport

all: $(ALL)

%.o : %.c
	$(CC) $(CFLAGS) $<

$(ALL) : % : %.o $(OBJECTS)       
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o


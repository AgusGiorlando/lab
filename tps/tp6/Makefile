CC = gcc
CFLAGS = -g -Wall

TARGETS = srv

all: $(TARGETS)

srv: srv.c service.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)


test: srv
	./srv


clean:
	rm -f $(TARGETS) *.o *~


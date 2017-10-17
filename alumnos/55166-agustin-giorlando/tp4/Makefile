#
# J. Federico Heranandez <federico.hernandez@um.edu.ar>
#

CC = gcc
CFLAGS = -g -Wall


TARGETS = webito

all: $(TARGETS)

webito: server.c service.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)
	

clean:
	rm -f $(TARGETS) *.o *~


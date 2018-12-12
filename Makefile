LDIR = lib
CC = gcc
CFLAGS = -I$(LDIR)

BDIR = bin
SDIR = src
TDIR = test
IDIR = include

_REQUESTER = connection.c error.c message.c crypt.c other.c
REQUESTER = $(patsubst %,$(IDIR)/%,$(_REQUESTER))

_WORKER = connection.c error.c message.c crypt.c other.c
WORKER = $(patsubst %,$(IDIR)/%,$(_WORKER))

_SERVER = connection.c error.c message.c other.c object.c structure.c
SERVER = $(patsubst %,$(IDIR)/%,$(_SERVER))

_TEST = connection.c error.c message.c other.c crypt.c object.c structure.c
TEST = $(patsubst %,$(IDIR)/%,$(_TEST))

request:
	@$(CC) $(CFLAGS) -o $(BDIR)/$@ $(SDIR)/requester.c $(REQUESTER) -lcrypt

worker:
	@$(CC) $(CFLAGS) -o $(BDIR)/$@ $(SDIR)/worker.c $(WORKER) -lcrypt -pthread

server:
	@$(CC) $(CFLAGS) -o $(BDIR)/$@ $(SDIR)/server.c $(SERVER) -pthread

test:
	@$(CC) $(CFLAGS) -o $(BDIR)/$@ $(SDIR)/test.c $(TEST) -lcrypt

all: request worker server

.PHONY: clean

clean:
	rm bin/*
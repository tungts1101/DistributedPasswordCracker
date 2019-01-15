CC = gcc
LDIR = lib
CFLAGS = -I$(LDIR)

BDIR = bin
SDIR = src
TDIR = test
IDIR = include

_REQUESTER = connection.c error.c message.c helper.c
REQUESTER = $(patsubst %,$(IDIR)/%,$(_REQUESTER))

_WORKER = connection.c error.c message.c helper.c
WORKER = $(patsubst %,$(IDIR)/%,$(_WORKER))

_SERVER = connection.c error.c message.c helper.c object.c structure.c
SERVER = $(patsubst %,$(IDIR)/%,$(_SERVER))

_TEST = connection.c error.c message.c helper.c object.c structure.c
TEST = $(patsubst %,$(IDIR)/%,$(_TEST))

create_bin_folder:
	@[ -d $(BDIR) ] || mkdir -p $(BDIR)

request: create_bin_folder
	@$(CC) $(CFLAGS) -o $(BDIR)/$@ $(SDIR)/requester.c $(REQUESTER) -lcrypt -std=gnu99

worker: create_bin_folder
	@$(CC) $(CFLAGS) -o $(BDIR)/$@ $(SDIR)/worker.c $(WORKER) -lcrypt -pthread -std=gnu99

server: create_bin_folder
	@$(CC) $(CFLAGS) -o $(BDIR)/$@ $(SDIR)/server.c $(SERVER) -lcrypt -pthread -std=gnu99

test: create_bin_folder
	@$(CC) $(CFLAGS) -o $(BDIR)/$@ $(SDIR)/test.c $(TEST) -lcrypt -std=gnu99

all: request worker server

.PHONY: clean

clean:
	rm bin/* logs
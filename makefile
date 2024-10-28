CC_OUTSPEC = -o $@
CC = gcc
BINDIR = /usr/local/bin
ITTY = itty.c
.PHONY: install

install: itty
	sudo install -m 0755 itty ${BINDIR}

itty: ${ITTY}
	$(CC) $(CC_OUTSPEC) $(ITTY) -lcurl

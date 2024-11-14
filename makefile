CC_OUTSPEC = -o $@
CC = gcc
BINDIR = /usr/local/bin
ITTY = itty.c
PCNTL = pcntl.c
.PHONY: install

install: itty pcntl
	sudo install -m 0755 itty ${BINDIR}
	sudo install -m 0755 pcntl $(BINDIR)

itty: ${ITTY}
	$(CC) $(CC_OUTSPEC) $(ITTY) -lcurl

pcntl: $(PCNTL)
	$(CC) $(CC_OUTSPEC) $(PCNTL)

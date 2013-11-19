CC=gcc
FLAGS=-Wall -O2

INSTALL=install

EXEC_NAME=beep
MAN_FILE=beep.1.gz

# Use GNU makefile conventions for directory names with one notable
# exception: prefix is not /usr/local in order to keep the default
# installation location for beep.
prefix=/usr
exec_prefix=$(prefix)
bindir=$(exec_prefix)/bin
datarootdir=$(prefix)/share
mandir=$(datarootdir)/man
man1dir=$(mandir)/man1

.PHONY: all
all: $(EXEC_NAME)

.PHONY: clean
clean:
	rm -f $(EXEC_NAME)

$(EXEC_NAME): beep.c
	$(CC) $(FLAGS) $(CFLAGS) -o $(EXEC_NAME) beep.c

install: all
	$(INSTALL) -m 0755 -d           $(DESTDIR)$(bindir)
	$(INSTALL) -m 0755 $(EXEC_NAME) $(DESTDIR)$(bindir)/
	$(INSTALL) -m 0755 -d           $(DESTDIR)$(man1dir)
	$(INSTALL) -m 0644 $(MAN_FILE)  $(DESTDIR)$(man1dir)/

uninstall:
	rm -f $(DESTDIR)$(bindir)/$(EXEC_NAME)
	rm -f $(DESTDIR)$(man1dir)/$(MAN_FILE)

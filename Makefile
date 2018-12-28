CC=gcc
FLAGS=-Wall -O2

GZIP = gzip
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
all: all-local


TARGETS += $(EXEC_NAME)
$(EXEC_NAME): beep.c
	$(CC) $(FLAGS) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $(EXEC_NAME) -lm $(LIBADD) beep.c

TARGETS += $(MAN_FILE)
$(MAN_FILE): beep.1
	$(GZIP) --best -c < $< > $@

all-local: $(TARGETS)

.PHONY: clean
clean:
	rm -f $(TARGETS)

install: all
	$(INSTALL) -m 0755 -d              $(DESTDIR)$(bindir)
	$(INSTALL) -m 0755 -p $(EXEC_NAME) $(DESTDIR)$(bindir)/
	$(INSTALL) -m 0755 -d              $(DESTDIR)$(man1dir)
	$(INSTALL) -m 0644 -p $(MAN_FILE)  $(DESTDIR)$(man1dir)/

uninstall:
	rm -f $(DESTDIR)$(bindir)/$(EXEC_NAME)
	rm -f $(DESTDIR)$(man1dir)/$(MAN_FILE)

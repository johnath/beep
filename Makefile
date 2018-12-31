# Use GNU makefile conventions for directory names with one notable
# exception: prefix is not /usr/local in order to keep the default
# installation location for beep.
DESTDIR=
prefix=/usr
exec_prefix=$(prefix)
bindir=$(exec_prefix)/bin
datarootdir=$(prefix)/share
mandir=$(datarootdir)/man
man1dir=$(mandir)/man1

CC=gcc
FLAGS=-Wall -O2

GZIP = gzip
INSTALL=install

EXEC_NAME=beep
MAN_FILE=beep.1.gz

.PHONY: all
all: all-local

TARGETS += $(EXEC_NAME)
$(EXEC_NAME): beep.c
	$(CC) $(FLAGS) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $(EXEC_NAME) -lm $(LIBADD) beep.c

TARGETS += $(MAN_FILE)
$(MAN_FILE): beep.1
	$(GZIP) --best -c < $< > $@

all-local: $(TARGETS)

HTML_TARGETS =
HTML_TARGETS += html/README.html
HTML_TARGETS += html/INSTALL.html

ifneq ($(shell if test -f /usr/bin/pandoc; then echo yes; fi),yes)
.PHONY: html
html:
	@echo "You need to install pandoc."
	@exit 1
else
.PHONY: html
html: $(HTML_TARGETS)
	mkdir -p html
	cp -f pandoc.css html/

html/%.html: %.md Makefile
	mkdir -p  html
	pandoc --from gfm --to html --standalone -M pagetitle="$$(sed -n 1p $<)" -M title="" -c pandoc.css $< -o $@
endif

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

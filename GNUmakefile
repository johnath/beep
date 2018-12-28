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

GCC = gcc
GCC_CFLAGS += -Wall -Wextra
GCC_CFLAGS += -std=gnu99 -pedantic
GCC_CFLAGS += -Werror
GCC_CFLAGS += -O -gstabs

CHECK_TARGETS += beep.gcc
beep.gcc: beep.c Makefile
	$(GCC) $(GCC_CPPFLAGS) $(GCC_CFLAGS) $(GCC_LDFLAGS) -o $@ -lm $(GCC_LIBADD) beep.c

CLANG = clang
CLANG_CFLAGS += -Wall -Wextra
CLANG_CFLAGS += -std=gnu99 -pedantic
CLANG_CFLAGS += -Werror
CLANG_CFLAGS += -fsanitize=undefined
CLANG_CFLAGS += -O -g

CHECK_TARGETS += beep.clang
beep.clang: beep.c Makefile
	$(CLANG) $(CLANG_CPPFLAGS) $(CLANG_CFLAGS) $(CLANG_LDFLAGS) -o $@ -lm $(CLANG_LIBADD) beep.c

TARGETS += $(MAN_FILE)
$(MAN_FILE): beep.1
	$(GZIP) --best -c < $< > $@

.PHONY: all-local
all-local: $(TARGETS)

HTML_TARGETS =
HTML_TARGETS += html/README.html
HTML_TARGETS += html/INSTALL.html
HTML_TARGETS += html/DEVELOPMENT.html

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

.PHONY: check
check: $(TARGETS) $(CHECK_TARGETS)
	/bin/bash tests/run-tests tests beep beep.clang beep.gcc

.PHONY: clean
clean:
	rm -f $(TARGETS) $(CHECK_TARGETS)
	rm -f tests/*.new tests/*.output.actual

.PHONY: install
install: all
	$(INSTALL) -m 0755 -d              $(DESTDIR)$(bindir)
	$(INSTALL) -m 0755 -p $(EXEC_NAME) $(DESTDIR)$(bindir)/
	$(INSTALL) -m 0755 -d              $(DESTDIR)$(man1dir)
	$(INSTALL) -m 0644 -p $(MAN_FILE)  $(DESTDIR)$(man1dir)/

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(bindir)/$(EXEC_NAME)
	rm -f $(DESTDIR)$(man1dir)/$(MAN_FILE)

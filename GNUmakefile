########################################################################
# Installation directories
########################################################################

# We use GNU makefile conventions for directory names with one notable
# exception: prefix is not /usr/local in order to keep the default
# installation location for beep.

PACKAGE_TARNAME = beep

DESTDIR=
prefix=/usr
exec_prefix=$(prefix)
bindir=$(exec_prefix)/bin
sbindir=$(exec_prefix)/sbin
datarootdir=$(prefix)/share
mandir=$(datarootdir)/man
man1dir=$(mandir)/man1
docdir=$(datarootdir)/doc
pkgdocdir=$(docdir)/$(PACKAGE_TARNAME)


########################################################################
# Tools
########################################################################

pathsearch = $(firstword $(wildcard $(addsuffix /$(1),$(subst :, ,$(PATH)))))

# Avoid running GNU make builtin rules by mistake
CC = false

GZIP = $(call pathsearch,gzip)
INSTALL = $(call pathsearch,install)
PANDOC = $(call pathsearch,pandoc)
SED = $(call pathsearch,sed)
SLOCCOUNT = $(call pathsearch,sloccount)


########################################################################
# This needs to be the first rule
########################################################################

.PHONY: all
all: all-local


########################################################################
# Variables to add to later
########################################################################

ALL_PROGRAMS =
bin_PROGRAMS =
sbin_PROGRAMS =
CLEANFILES =
HTML_DATA =
man1_DATA =
pkgdoc_DATA =


########################################################################
# Define compilers and their flags
########################################################################

COMPILER_gcc = gcc
LINKER_gcc = gcc
CPPFLAGS_gcc =
CFLAGS_gcc += -Wall -Wextra
CFLAGS_gcc += -std=gnu99 -pedantic
CFLAGS_gcc += -Werror
CFLAGS_gcc += -O -gstabs
CFLAGS_gcc += -Werror=format-security
CFLAGS_gcc += -Wp,-D_FORTIFY_SOURCE=2
CFLAGS_gcc += -Wp,-D_GLIBCXX_ASSERTIONS
CFLAGS_gcc += -fstack-protector-strong
CFLAGS_gcc += -fasynchronous-unwind-tables
CFLAGS_gcc += -fstack-clash-protection
CFLAGS_gcc += -fcf-protection
CFLAGS_gcc += -save-temps=obj
LDFLAGS_gcc =
LIBS_gcc =

ifneq ($(call pathsearch,$(COMPILER_gcc)),)
ifneq ($(COMPILER_gcc)),no)
COMPILERS += gcc
endif
endif

COMPILER_clang = clang
LINKER_clang = clang
CPPFLAGS_clang =
CFLAGS_clang += -Wall -Wextra
CFLAGS_clang += -std=gnu99 -pedantic
CFLAGS_clang += -Werror
CFLAGS_clang += -fsanitize=undefined
CFLAGS_clang += -O -g
LDFLAGS_clang =
LIBS_clang =

ifneq ($(call pathsearch,$(COMPILER_clang)),)
ifneq ($(COMPILER_clang),no)
COMPILERS += clang
endif
endif


########################################################################
# Detect libraries
########################################################################


########################################################################
# Define executables and their flags
########################################################################

bin_PROGRAMS += beep
beep_OBJS =
beep_OBJS += beep.o
beep_OBJS += beep-log.o
beep_OBJS += beep-usage.o
beep_LIBS =
beep_LIBS += -lm

# sbin_PROGRAMS += beep2
# beep2_OBJS =
# beep2_OBJS += beep.o
# beep2_OBJS += beep-log.o
# beep2_LIBS =
# beep2_LIBS += -lm


########################################################################
# Built sources
########################################################################

CLEANFILES += beep-usage.c
beep-usage.c: beep-usage.txt
	echo '/* Auto-generated from beep-usage.txt. Modify that file instead. */' > $@
	echo '#include "beep-usage.h"' >> $@
	echo 'char beep_usage[] =' >> $@
	set -e; IFS=""; while read line; do \
		echo "  \"$${line}\\n\"" >> $@; \
	done < $<
	echo '  ;' >> $@


########################################################################
# Compile and Link rules including automatic dependency generation
########################################################################

# CALL: LINK_RULE <compiler> <executable>
define LINK_RULE
ALL_PROGRAMS += $(2).$(1)

$(2).$(1): $(patsubst %.o,%.$(1)-o,$($(2)_OBJS))
	@: echo "LINK_RULE $$@: $$^"
	$(LINKER_$(1)) $(CFLAGS) $(CFLAGS_$(1)) $(LDFLAGS) $(LDFLAGS_$(1)) -o $$@ $($(2)_LIBS) $(LIBS_$(1)) $(LIBS) $$^
endef

# CALL: PER_COMPILER <compiler>
define PER_COMPILER
$(foreach exec,$(bin_PROGRAMS) $(sbin_PROGRAMS),$(eval $(call LINK_RULE,$(1),$(exec))))

%.$(1)-o: %.c
	$$(COMPILER_$(1)) $$(CPPFLAGS) $$(CPPFLAGS_$(1)) $$(CFLAGS) $$(CFLAGS_$(1)) -o $$@ -c $$<

%.$(1)-o.dep: %.c
	$$(COMPILER_$(1)) $$(CPPFLAGS) $$(CPPFLAGS_$(1)) $$(CFLAGS) $$(CFLAGS_$(1)) -MM -MT "$$*.$(1)-o $$@ " $$< > $$@.tmp
	mv -f $$@.tmp $$@

-include $$(patsubst %.o,%.$(1)-o.dep,$(beep_OBJS))
endef

$(foreach compiler,$(COMPILERS),$(eval $(call PER_COMPILER,$(compiler))))

# For each executable, take the first from COMPILERS to use
%: $(firstword $(foreach comp,$(COMPILERS),%.$(comp)))
	cp -f $< $@


########################################################################
# Generate doc and similar files
########################################################################

man1_DATA  += beep.1.gz
CLEANFILES += beep.1.gz

%.1.gz: %.1
	$(GZIP) --best -c < $< > $@

HTML_DATA += html/README.html
HTML_DATA += html/INSTALL.html
HTML_DATA += html/DEVELOPMENT.html

.PHONY: html
html: $(HTML_DATA)
	@mkdir -p html
	cp -f pandoc.css html/

html/%.html: %.md
	@mkdir -p  html
	@if test -f $(PANDOC); then \
		echo PANDOC $< -o $@; \
		$(PANDOC) --from gfm --to html --standalone -M pagetitle="$$($(SED) -n 1p $<)" -M title="" -c pandoc.css $< -o $@; \
	else \
		echo "You need to install pandoc to generate the HTML files."; \
		exit 1; \
	fi

pkgdoc_DATA += README.md

########################################################################
# Generic targets
########################################################################

.PHONY: all-local
all-local: $(bin_PROGRAMS) $(sbin_PROGRAMS) $(ALL_PROGRAMS) $(man1_DATA)

SLOC_SOURCES =
SLOC_SOURCES += beep*.[ch]
SLOC_SOURCES += gen-freq-table
SLOC_SOURCES += tests/run-tests
SLOC_SOURCES += tests/*.sh
SLOC_SOURCES += GNUmakefile

.PHONY: sloccount
sloccount:
	@if test -e $(SLOCCOUNT); then \
		$(SLOCCOUNT) --details $(SLOC_SOURCES); \
		$(SLOCCOUNT) $(SLOC_SOURCES); \
	else \
		echo "sloccount not found"; \
	fi

SPLINT = splint
SPLINT_FLAGS += -standard
# SPLINT_FLAGS += -checks
# SPLINT_FLAGS += -strict
SPLINT_FLAGS += +posixstrictlib
SPLINT_FLAGS += +gnuextensions
SPLINT_FLAGS += -preproc
SPLINT_FLAGS += -syntax
SPLINT_FLAGS += -D__signed__=signed

.PHONY: lint
lint:
	$(SPLINT) $(SPLINT_FLAGS) beep*.c beep*.h

.PHONY: check-targets
check-targets: $(TARGETS) $(CHECK_TARGETS)

.PHONY: check
check: beep $(ALL_PROGRAMS)
	/bin/bash tests/run-tests tests $(foreach compiler,$(COMPILERS),beep.$(compiler))

.PHONY: clean
clean:
	rm -f $(bin_PROGRAMS) $(sbin_PROGRAMS)
	rm -f $(CLEANFILES)
	rm -f $(foreach comp,$(COMPILERS),*.$(comp) *.$(comp)-o)
	rm -f *.dep
	rm -f tests/*.new tests/*.output.actual
	rm -rf html
	rm -f *.o *.i *.s

.PHONY: doc
doc: $(pkgdoc_DATA)

.PHONY: install
install: all
ifneq (,$(bin_PROGRAMS))
	$(INSTALL) -m 0755 -d                 $(DESTDIR)$(bindir)
	$(INSTALL) -m 0755 -p $(bin_PROGRAMS) $(DESTDIR)$(bindir)/
endif
ifneq (,$(sbin_PROGRAMS))
	$(INSTALL) -m 0755 -d                  $(DESTDIR)$(sbindir)
	$(INSTALL) -m 0755 -p $(sbin_PROGRAMS) $(DESTDIR)$(sbindir)/
endif
ifneq (,$(man1_DATA))
	$(INSTALL) -m 0755 -d              $(DESTDIR)$(man1dir)
	$(INSTALL) -m 0644 -p $(man1_DATA) $(DESTDIR)$(man1dir)/
endif

.PHONY: install-doc
install-doc: doc
ifneq (,$(pkgdoc_DATA))
	$(INSTALL) -m 0755 -d                $(DESTDIR)$(pkgdocdir)
	$(INSTALL) -m 0644 -p $(pkgdoc_DATA) $(DESTDIR)$(pkgdocdir)/
endif

.PHONY: uninstall
uninstall:
	for f in $(bin_PROGRAMS);  do rm -f "$(DESTDIR)$(bindir)/$$f";    done
	for f in $(sbin_PROGRAMS); do rm -f "$(DESTDIR)$(sbindir)/$$f";   done
	for f in $(man1_DATA);     do rm -f "$(DESTDIR)$(man1dir)/$$f";   done
	for f in $(pkgdoc_DATA);   do rm -f "$(DESTDIR)$(pkgdocdir)/$$f"; done


########################################################################
# Development helpers
########################################################################

# List all references to documentation in the git repo
.PHONY: refs
refs:
	git grep -E '((http|https)://[a-zA-Z0-9\._/-]+|([A-Z]+\.md)|([a-zA-Z][a-zA-Z0-9_-]+\([0-9]+\)))'

# List all TODOs and FIXMEs in the git repo
.PHONY: todo
todo:
	git grep -E '(TODO|FIXME):'


########################################################################
# End of GNUmakefile
########################################################################

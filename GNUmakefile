########################################################################
# Installation directories
########################################################################

# We use GNU makefile conventions for directory names with one notable
# exception: prefix is not /usr/local in order to keep the traditional
# default installation location for beep.

PACKAGE_TARNAME = beep
PACKAGE_VERSION = 1.4.9

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
contribdir=$(pkgdocdir)/contrib


########################################################################
# Tools
########################################################################

pathsearch = $(firstword $(wildcard $(addsuffix /$(1),$(subst :, ,$(PATH)))))

# Avoid running GNU make builtin rules based on $(CC) by mistake
CC = false

DOT = $(call pathsearch,dot)
DOXYGEN = $(call pathsearch,doxygen)
EGREP = $(GREP) -E
GIT = $(call pathsearch,git)
GREP = $(call pathsearch,grep)
INSTALL = $(call pathsearch,install)
MKDIR_P = mkdir -p
PANDOC = $(call pathsearch,pandoc)
PYTHON3 = $(call pathsearch,python3)
SED = $(call pathsearch,sed)
SLOCCOUNT = $(call pathsearch,sloccount)


########################################################################
# This needs to be the first rule
########################################################################

.PHONY: all
all: all-local


# Have make forget rules for known suffixes
.SUFFIXES:


########################################################################
# Variables to add to later
########################################################################

# targets to build for the "all" target
all_TARGETS =

# targets to build for the "check" target
check_TARGETS =

bin_PROGRAMS =
check_PROGRAMS =
contrib_DATA =
contrib_SCRIPTS =
sbin_PROGRAMS =
CLEANFILES =
HTML_DATA =
man1_DATA =
pkgdoc_DATA =


########################################################################
# Define compilers and their flags
########################################################################

# CPPFLAGS common to all compilers
CPPFLAGS_COMMON = 
CPPFLAGS_COMMON += -DPACKAGE_TARNAME='"$(PACKAGE_TARNAME)"'
CPPFLAGS_COMMON += -DPACKAGE_VERSION='"$(PACKAGE_VERSION)"'

comma := ,

# If supported by COMPILER_gcc, add given flags to CFLAGS_gcc.
# Example usage:
#   $(eval $(call CHECK_CFLAGS_gcc,-fasynchronous-unwind-tables))
define CHECK_CFLAGS_gcc
CFLAGS_gcc += $$(if $$(shell if $$(COMPILER_gcc) $(1) -x c -o compile-check.gcc-o -c - < /dev/null > /dev/null 2>&1; then echo yes; else :; fi; rm -f compile-check.gcc-o > /dev/null 2>&1),$(1))
endef

COMPILER_gcc = gcc
LINKER_gcc = gcc
CPPFLAGS_gcc =
CFLAGS_gcc =
CFLAGS_gcc += -std=gnu99 -pedantic
CFLAGS_gcc += -O -g
CFLAGS_gcc += -Wa,-adhlns=$(@:-o=-lst)
$(eval $(call CHECK_CFLAGS_gcc,-Wall -Wextra -Werror -Werror=format-security))
$(eval $(call CHECK_CFLAGS_gcc,-Wp$$(comma)-D_FORTIFY_SOURCE=2))
$(eval $(call CHECK_CFLAGS_gcc,-Wp$$(comma)-D_GLIBCXX_ASSERTIONS))
$(eval $(call CHECK_CFLAGS_gcc,-fasynchronous-unwind-tables))
$(eval $(call CHECK_CFLAGS_gcc,-fstack-protector-strong))
$(eval $(call CHECK_CFLAGS_gcc,-fstack-clash-protection))
$(eval $(call CHECK_CFLAGS_gcc,-fcf-protection))
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
CFLAGS_clang += -Weverything
CFLAGS_clang += -Wno-padded
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

check_PROGRAMS        += issue-6-benchmark
issue_6_benchmark_OBJS = issue-6-benchmark.o
issue_6_benchmark_LIBS = -lm

bin_PROGRAMS += beep
beep_OBJS =
beep_OBJS += beep-log.o
beep_OBJS += beep-main.o
beep_OBJS += beep-library.o
beep_OBJS += beep-usage.o
beep_OBJS += beep-drivers.o

# The drivers here use `__attribute__((constructor))` functions to
# register themselves with `beep_drivers_register()`, so the last one
# listed here will be linked last and have its constructor called at
# the latest time, and thus will have its `driver_detect()` function
# called first.

beep_OBJS += beep-driver-console.o
beep_OBJS += beep-driver-evdev.o
ifneq ($(BEEP_DEBUG_BUILD),)
beep_OBJS += beep-driver-noop.o
endif

beep_LIBS =

beep-log.clang-o : override CFLAGS_clang += -D_GNU_SOURCE
beep-log.gcc-o   : override CFLAGS_gcc   += -D_GNU_SOURCE

beep-log.clang-o : override CFLAGS_clang += -Wno-format-nonliteral

# sbin_PROGRAMS += beep-foo
# beep_foo_OBJS =
# beep_foo_OBJS += beep-log.o
# beep_foo_OBJS += beep.o
# beep_foo_LIBS =
# beep_foo_LIBS += -lm


########################################################################
# Built sources
########################################################################

CLEANFILES += beep-usage.c
beep-usage.c: beep-usage.txt
	echo '/* Auto-generated from `$<`. Modify that file instead. */' > $@
	echo '#include "beep-usage.h"' >> $@
	echo 'char beep_usage[] =' >> $@
	set -e; IFS=""; while read line; do \
		printf '  "%s\\n"\n' "$${line}" >> $@; \
	done < $<
	echo '  ;' >> $@

CLEANFILES += beep-usage.txt


########################################################################
# Compile and Link rules including automatic dependency generation
#
# For details on the automatic dependency generation, see
# http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
########################################################################


# CALL: LINK_RULE <compiler> <executable> <executable_as_varname_part> <dircomponent>
# To be called from PER_COMPILER. Defines the per-executable rules.
define LINK_RULE
$(4)_ALL_PROGRAMS += $(2).$(1)

$(2).$(1): $(patsubst %.o,%.$(1)-o,$($(3)_OBJS))
	@: echo "LINK_RULE $$@: $$^"
	$(LINKER_$(1)) $(CFLAGS) $(CFLAGS_$(1)) $(LDFLAGS) $(LDFLAGS_$(1)) -o $$@ $$^ $($(3)_LIBS) $(LIBS_$(1)) $(LIBS)

$$(patsubst %.o,.deps/%.$(1)-o.dep,$($(3)_OBJS))):

-include $$(wildcard $$(patsubst %.o,.deps/%.$(1)-o.dep,$($(3)_OBJS)))
endef


# CALL: PER_COMPILER <compiler>
# To be called for each compiler. Defines the per-compiler rules for each executable.
define PER_COMPILER
$(foreach exec,$(bin_PROGRAMS),$(eval $(call LINK_RULE,$(1),$(exec),$(subst -,_,$(exec)),bin)))
$(foreach exec,$(check_PROGRAMS),$(eval $(call LINK_RULE,$(1),$(exec),$(subst -,_,$(exec)),check)))
$(foreach exec,$(sbin_PROGRAMS),$(eval $(call LINK_RULE,$(1),$(exec),$(subst -,_,$(exec)),sbin)))

%.$(1)-o: %.c | .deps
	$$(COMPILER_$(1)) -MT $$@ -MMD -MP -MF .deps/$$*.$(1)-o.dep $$(CPPFLAGS) $$(CPPFLAGS_COMMON) $$(CPPFLAGS_$(1)) $$(CFLAGS_COMMON) $$(CFLAGS) $$(CFLAGS_$(1)) -o $$@ -c $$<

%.h-check.$(1)-o: %.h | .deps
	$$(COMPILER_$(1)) -MT $$@ -MMD -MP -MF .deps/$$*.h-check.$(1)-o.dep $$(CPPFLAGS) $$(CPPFLAGS_COMMON) $$(CPPFLAGS_$(1)) $$(CFLAGS_COMMON) $$(CFLAGS) $$(CFLAGS_$(1)) -o $$@ -c $$<
endef

$(foreach compiler,$(COMPILERS),$(eval $(call PER_COMPILER,$(compiler))))

.deps:
	@$(MKDIR_P) $@


# For each executable, take the first from COMPILERS to determine the
# variant to use as the default executables.
%: $(firstword $(foreach comp,$(COMPILERS),%.$(comp)))
	cp -f $< $@


########################################################################
# Generate doc and similar files
########################################################################

man1_DATA  += beep.1
CLEANFILES += beep.1

HTML_DATA += html/CREDITS.html
HTML_DATA += html/DEVELOPMENT.html
HTML_DATA += html/INSTALL.html
HTML_DATA += html/NEWS.html
HTML_DATA += html/PACKAGING.html
HTML_DATA += html/PERMISSIONS.html
HTML_DATA += html/README.html

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

DEFAULT_FREQ   = 440
DEFAULT_LENGTH = 200
DEFAULT_DELAY  = 100

REPLACEMENTS =
REPLACEMENTS += -e s/@PACKAGE_TARNAME@/$(PACKAGE_TARNAME)/g
REPLACEMENTS += -e s/@PACKAGE_VERSION@/$(PACKAGE_VERSION)/g

REPLACEMENTS += -e s/@DEFAULT_FREQ@/$(DEFAULT_FREQ)/g
REPLACEMENTS += -e s/@DEFAULT_LENGTH@/$(DEFAULT_LENGTH)/g
REPLACEMENTS += -e s/@DEFAULT_DELAY@/$(DEFAULT_DELAY)/g

REPLACEMENTS += -e 's|[@]pkgdocdir@|$(pkgdocdir)|g'

CLEANFILES    += beep-config.h
BUILT_SOURCES += beep-config.h
beep-main.clang-o : beep-config.h
beep-main.gcc-o : beep-config.h

CLEANFILES += Doxyfile
CLEANFILES += Doxyfile.new

%: %.in GNUmakefile
	$(SED) $(REPLACEMENTS) < $< > $@.new
	@if $(EGREP) '@([A-Za-z][A-Za-z0-9_]*)@' $@.new; then \
		echo "Error: GNUmakefile fails to substitute some of the variables in \`$<'."; \
		exit 1; \
	fi
	mv -f $@.new $@

CLEANFILES += doxygen.stamp
.PHONY: doxygen.stamp
doxygen.stamp: Doxyfile $(wildcard *.c) $(wildcard *.h)
	$(DOXYGEN) $<
	echo > $@

html: doxygen.stamp

.PHONY: serve-html
serve-html: html
	$(PYTHON3) -m http.server --directory html/dox/html

pkgdoc_DATA += COPYING
pkgdoc_DATA += CREDITS.md
pkgdoc_DATA += NEWS.md
pkgdoc_DATA += README.md
pkgdoc_DATA += PERMISSIONS.md

contrib_SCRIPTS += contrib/failure-beeps
contrib_SCRIPTS += contrib/success-beeps


########################################################################
# Generic targets
########################################################################

all_TARGETS += $(bin_PROGRAMS) $(bin_ALL_PROGRAMS)
all_TARGETS += $(sbin_PROGRAMS) $(sbin_ALL_PROGRAMS)
all_TARGETS += $(man1_DATA)

.PHONY: all-local
all-local: $(all_TARGETS)

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
SPLINT_FLAGS += $(CPPFLAGS_COMMON)

.PHONY: lint
lint:
	$(SPLINT) $(SPLINT_FLAGS) beep*.c beep*.h

check_TARGETS += $(all_TARGETS)
check_TARGETS += $(check_PROGRAMS) $(check_ALL_PROGRAMS)
check_TARGETS += $(foreach compiler,$(COMPILERS),$(foreach header,$(wildcard beep-*.h),$(basename $(header)).h-check.$(compiler)-o))

.PHONY: check-targets
check-targets: $(check_TARGETS)

.PHONY: check
check: $(foreach compiler,$(COMPILERS),beep.$(compiler)) $(check_TARGETS)
	env PACKAGE_VERSION="${PACKAGE_VERSION}" \
	/bin/bash tests/run-tests tests $(foreach compiler,$(COMPILERS),beep.$(compiler))

.PHONY: clean
clean:
	rm -f $(bin_PROGRAMS) $(bin_ALL_PROGRAMS)
	rm -f $(check_PROGRAMS) $(check_ALL_PROGRAMS)
	rm -f $(sbin_PROGRAMS) $(sbin_ALL_PROGRAMS)
	rm -f $(CLEANFILES)
	rm -f $(foreach comp,$(COMPILERS),*.$(comp)-o)
	rm -f *.dep
	rm -rf .deps
	rm -f *.lst *.gcc-lst
	rm -f tests/*.new tests/*.actual
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
ifneq (,$(pkgdoc_DATA))
	$(INSTALL) -m 0755 -d                $(DESTDIR)$(pkgdocdir)
	$(INSTALL) -m 0644 -p $(pkgdoc_DATA) $(DESTDIR)$(pkgdocdir)/
endif
ifneq (,$(contrib_DATA)$(contrib_SCRIPTS))
	$(INSTALL) -m 0755 -d                    $(DESTDIR)$(contribdir)
endif
ifneq (,$(contrib_DATA))
	$(INSTALL) -m 0644 -p $(contrib_DATA)    $(DESTDIR)$(contribdir)/
endif
ifneq (,$(contrib_SCRIPTS))
	$(INSTALL) -m 0755 -p $(contrib_SCRIPTS) $(DESTDIR)$(contribdir)/
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

# Only have make deal with those variables and rules if this is a git
# repo, and if the git executable has been found.
GIT_INFO_EXCLUDE = $(firstword $(wildcard .git/info/exclude))
ifneq ($(strip $(GIT_INFO_EXCLUDE)),)
ifdef GIT

# List all references to documentation in the git repo
.PHONY: refs
refs:
	$(GIT) grep -n -E '((http|https)://[a-zA-Z0-9\._/-]+|([A-Z]+\.md)|([a-zA-Z][a-zA-Z0-9_-]+\([0-9]+\)))'

# List all TODOs and FIXMEs in the git repo
.PHONY: todo fixme
todo fixme:
	$(GIT) grep -n -E '(TODO:|FIXME:|\\todo\s|@todo\s)'

# Generate a kind of dist tarball to help with preparing for release
PACKAGE_TARBASE := $(PACKAGE_TARNAME)-$(shell $(GIT) describe --tags | $(SED) 's/^v\([0-9]\)/\1/')
.PHONY: dist
dist:
	$(GIT) archive --format=tar.gz --verbose --prefix=$(PACKAGE_TARBASE)/ --output=$(PACKAGE_TARBASE).tar.gz HEAD

endif
endif


########################################################################
# End of GNUmakefile
########################################################################

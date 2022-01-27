########################################################################
# GNUmakefile for beep
#
# This GNUmakefile has been written to mostly follow the GNU Makefile
# conventions[1], even if there is no "configure" script to accompany
# the GNUmakefile. If you want to keep some definitions across "make"
# invocations instead of providing them on each "make" command line,
# write the definitions into a file "local.mk" make include file, e.g.
#
#     CC = clang
#     prefix = $(HOME)/foo-prefix
#
# The GNUmakefile also internally follows some conventions known from
# Automake (e.g. bin_PROGRAMS variables), but uses features specific
# to GNU make to implement those Automake like features.
#
# [1]: https://www.gnu.org/prep/standards/html_node/Makefile-Conventions.html
########################################################################


$(info #=======================================================================)


########################################################################
# Package metadata
########################################################################

PACKAGE_TARNAME = beep
PACKAGE_VERSION = 1.4.12


########################################################################
# Installation directories
########################################################################

# We use GNU makefile conventions for directory names.

prefix      = /usr/local
exec_prefix = $(prefix)
bindir      = $(exec_prefix)/bin
sbindir     = $(exec_prefix)/sbin
datarootdir = $(prefix)/share
mandir      = $(datarootdir)/man
man1dir     = $(mandir)/man1
docdir      = $(datarootdir)/doc/$(PACKAGE_TARNAME)
contribdir  = $(docdir)/contrib
htmldir     = $(docdir)

dir-vars  =
dir-vars += bindir
dir-vars += sbindir
dir-vars += datarootdir
dir-vars += mandir
dir-vars += man1dir
dir-vars += docdir
dir-vars += contribdir
dir-vars += htmldir

# TODO: We might want to autodetect which kind of $(docdir) is used on
#       this system.  Until then, people will just need to set
#       docdir='$(datarootdir)/doc/$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)'
#       if they want to use that.  Note that what the GNU makefile
#       conventions call $(docdir) is called other names by other
#       software components, e.g. %{_pkgdocdir} in RPM spec files.


########################################################################
# Tools
########################################################################

# This makes it easy to replace any of those tools with specific
# versions, e.g. to test the portability of the SED commands, you can
# run with SED="busybox sed".

DOT       = dot
DOXYGEN   = doxygen
EGREP     = $(GREP) -E
GIT       = git
GREP      = grep
INSTALL   = install
MKDIR_P   = mkdir -p
PANDOC    = pandoc
PYTHON3   = python3
SED       = sed


########################################################################
# This needs to be the first rule
########################################################################

.PHONY: all
all: all-local


# Prevent make from using its built-in rules
.SUFFIXES:
COMPILE.c = false COMPILE.c
LINK.c    = false LINK.c


########################################################################
# Variables to add to later
########################################################################

# targets to build for the "all" target
all_TARGETS      =

# targets to build for the "check" target
check_TARGETS    =

bin_PROGRAMS     =
check_PROGRAMS   =
contrib_DATA     =
contrib_SCRIPTS  =
sbin_PROGRAMS    =
CLEANFILES       =
html_DATA        =
noinst_html_DATA =
man1_DATA        =
doc_DATA         =


########################################################################
# Define compiler and linker flags
########################################################################

comma := ,

# If supported by $(CC), add given flags to CFLAGS type variable.
# Example usage:
#   $(eval $(call CHECK_CFLAGS,common-CFLAGS,-fasynchronous-unwind-tables))
define CHECK_CFLAGS
$(1) += $$(if $$(shell if $$(CC) $$(patsubst -Wno-%,-W%,$(2)) -x c -o compile-check.o -c - < /dev/null > /dev/null 2>&1; then echo yes; else :; fi; rm -f compile-check.o > /dev/null 2>&1),$(2))
endef

# This might be useful or not.
CFLAGS   :=
CPPFLAGS  =
LDFLAGS   =
LIBS      =

# Flags common to all executable targets
common_CFLAGS   :=
common_CPPFLAGS  =
common_LDFLAGS   =
common_LIBS      =

common_CPPFLAGS += -DPACKAGE_TARNAME='"$(PACKAGE_TARNAME)"'
common_CPPFLAGS += -DPACKAGE_VERSION='"$(PACKAGE_VERSION)"'
common_CFLAGS   += -O2 -g
common_CFLAGS   += -std=gnu99
common_CFLAGS   += $(if $(filter %.o,$@),-Wa$(comma)-adhlns=$(@:.o=.lst))
common_CFLAGS   += -pedantic
$(eval $(call CHECK_CFLAGS,common_CFLAGS,-Werror=unknown-warning-option))
$(eval $(call CHECK_CFLAGS,common_CFLAGS,-Wall))
$(eval $(call CHECK_CFLAGS,common_CFLAGS,-Wextra))
$(eval $(call CHECK_CFLAGS,common_CFLAGS,-Weverything))
$(eval $(call CHECK_CFLAGS,common_CFLAGS,-Werror))
$(eval $(call CHECK_CFLAGS,common_CFLAGS,-Wno-padded))
$(eval $(call CHECK_CFLAGS,common_CFLAGS,-Werror=format-security))
$(eval $(call CHECK_CFLAGS,common_CFLAGS,-Wno-disabled-macro-expansion))
$(eval $(call CHECK_CFLAGS,common_CFLAGS,-Wno-format-nonliteral))
$(eval $(call CHECK_CFLAGS,CPPFLAGS,-D_FORTIFY_SOURCE=2))
$(eval $(call CHECK_CFLAGS,CPPFLAGS,-D_GLIBCXX_ASSERTIONS))
# $(eval $(call CHECK_CFLAGS,CFLAGS,-Wp$$(comma)-D_FORTIFY_SOURCE=2))
# $(eval $(call CHECK_CFLAGS,CFLAGS,-Wp$$(comma)-D_GLIBCXX_ASSERTIONS))
$(eval $(call CHECK_CFLAGS,CFLAGS,-fasynchronous-unwind-tables))
$(eval $(call CHECK_CFLAGS,CFLAGS,-fanalyzer))
$(eval $(call CHECK_CFLAGS,CFLAGS,-fstack-protector-strong))
$(eval $(call CHECK_CFLAGS,CFLAGS,-fstack-clash-protection))
$(eval $(call CHECK_CFLAGS,CFLAGS,-fcf-protection))
$(eval $(call CHECK_CFLAGS,CFLAGS,-fsanitize=undefined))


CFLAGS += -save-temps=obj


$(info # common_CFLAGS=$(common_CFLAGS))
$(info # CFLAGS=$(CFLAGS))
$(info # CPPFLAGS=$(CPPFLAGS))


# Create this file to override any of the make variables defined
# above.
-include local.mk


########################################################################
# Detect libraries
########################################################################


########################################################################
# Define executables and their flags
########################################################################

check_PROGRAMS         += issue-6-benchmark
issue_6_benchmark_OBJS  = issue-6-benchmark.o
issue_6_benchmark_LIBS  = -lm

bin_PROGRAMS += beep
beep_OBJS     =
beep_OBJS    += beep-log.o
beep_OBJS    += beep-main.o
beep_OBJS    += beep-library.o
beep_OBJS    += beep-usage.o
beep_OBJS    += beep-drivers.o

# The drivers here use `__attribute__((constructor))` functions to
# register themselves with `beep_drivers_register()`, so the last one
# listed here will be linked last and have its constructor called at
# the latest time, and thus will have its `driver_detect()` function
# called first.

beep_OBJS    += beep-driver-console.o
beep_OBJS    += beep-driver-evdev.o
ifneq ($(BEEP_DEBUG_BUILD),)
beep_OBJS    += beep-driver-noop.o
endif

beep_LIBS     =

beep-log.o : override common_CPPFLAGS += -D_GNU_SOURCE
# beep-log.clang-o : override CFLAGS_clang += -Wno-format-nonliteral

# sbin_PROGRAMS += beep-foo
# beep_foo_OBJS  =
# beep_foo_OBJS += beep-log.o
# beep_foo_OBJS += beep.o
# beep_foo_LIBS  =
# beep_foo_LIBS += -lm


########################################################################
# Built sources
########################################################################

CLEANFILES += beep-usage.txt

CLEANFILES += beep-usage.c
beep-usage.c: beep-usage.txt
	echo '/* Auto-generated from `$<`. Modify that file instead. */' > $@
	echo '#include "beep-usage.h"' >> $@
	echo 'char beep_usage[] =' >> $@
	set -e; IFS=""; while read line; do \
		printf '  "%s\\n"\n' "$${line}" >> $@; \
	done < $<
	echo '  ;' >> $@


########################################################################
# Compile and Link rules including automatic dependency generation
#
# For details on the automatic dependency generation, see
# http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
########################################################################


# CALL: LINK_RULE <executable> <executable_as_varname_part> <dircomponent>
# Defines the per-executable rules.
define LINK_RULE
$(1): $$($(2)_OBJS)
	@: echo "LINK_RULE $$@: $$^"
	$(CC) -Wl,-Map=$$(@:%=%.map),--cref $(CFLAGS) $(common_CFLAGS) $(LDFLAGS) $(common_LDFLAGS) -o $$@ $$^ $$($(2)_LIBS) $(common_LIBS) $(LIBS)

$$(patsubst %.o,.deps/%.o.dep,$$($(2)_OBJS))):

-include $$(wildcard $$(patsubst %.o,.deps/%.o.dep,$$($(2)_OBJS)))
endef


$(foreach exec,$(bin_PROGRAMS),  $(eval $(call LINK_RULE,$(exec),$(subst -,_,$(exec)),bin)))
$(foreach exec,$(check_PROGRAMS),$(eval $(call LINK_RULE,$(exec),$(subst -,_,$(exec)),check)))
$(foreach exec,$(sbin_PROGRAMS), $(eval $(call LINK_RULE,$(exec),$(subst -,_,$(exec)),sbin)))

%.o: %.c | .deps
	$(CC) -MT $@ -MMD -MP -MF .deps/$*.o.dep $(common_CPPFLAGS) $(CPPFLAGS) $(common_CFLAGS) $(CFLAGS) -o $@ -c $<

.deps:
	@$(MKDIR_P) $@


########################################################################
# Generate doc and similar files
########################################################################

man1_DATA  += beep.1
CLEANFILES += beep.1

CLEANFILES       += CREDITS.html
CLEANFILES       += NEWS.html
CLEANFILES       += PERMISSIONS.html
CLEANFILES       += README.html

CLEANFILES       += DEVELOPMENT.html
CLEANFILES       += INSTALL.html
CLEANFILES       += PACKAGING.html

ifeq (yes,$(shell if $(PANDOC) --version > /dev/null 2>&1; then echo yes; else echo no; fi))

html_DATA        += pandoc.css

html_DATA        += CREDITS.html
html_DATA        += NEWS.html
html_DATA        += PERMISSIONS.html
html_DATA        += README.html

noinst_html_DATA += DEVELOPMENT.html
noinst_html_DATA += INSTALL.html
noinst_html_DATA += PACKAGING.html

%.html: %.md
	@echo PANDOC $< -o $@
	$(PANDOC) --from gfm --to html --standalone -M pagetitle="$$($(SED) -n 1p $<)" -M title="" -c pandoc.css $< -o $@
endif

.PHONY: html
html:      $(html_DATA) $(noinst_html_DATA)
all-local: $(html_DATA) $(noinst_html_DATA)

DEFAULT_FREQ   = 440
DEFAULT_LENGTH = 200
DEFAULT_DELAY  = 100

REPLACEMENTS  =
REPLACEMENTS += -e 's|@PACKAGE_TARNAME@|$(PACKAGE_TARNAME)|g'
REPLACEMENTS += -e 's|@PACKAGE_VERSION@|$(PACKAGE_VERSION)|g'

REPLACEMENTS += -e 's|@DEFAULT_FREQ@|$(DEFAULT_FREQ)|g'
REPLACEMENTS += -e 's|@DEFAULT_LENGTH@|$(DEFAULT_LENGTH)|g'
REPLACEMENTS += -e 's|@DEFAULT_DELAY@|$(DEFAULT_DELAY)|g'

REPLACEMENTS += -e 's|[@]docdir@|$(docdir)|g'

CLEANFILES    += beep-config.h
BUILT_SOURCES += beep-config.h
beep-main.o : beep-config.h

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

dox: doxygen.stamp

.PHONY: serve-dox
serve-dox: dox
	$(PYTHON3) -m http.server --directory dox/html

doc_DATA += COPYING
doc_DATA += CREDITS.md
doc_DATA += NEWS.md
doc_DATA += README.md
doc_DATA += PERMISSIONS.md

contrib_SCRIPTS += contrib/failure-beeps
contrib_SCRIPTS += contrib/success-beeps
contrib_SCRIPTS += contrib/morse/morse2beep.pl
contrib_SCRIPTS += contrib/morse/morse2beep.sed


########################################################################
# Generic targets
########################################################################

all_TARGETS += $(bin_PROGRAMS)
all_TARGETS += $(sbin_PROGRAMS)
all_TARGETS += $(man1_DATA)

.PHONY: all-local
all-local: $(all_TARGETS)

check_TARGETS += $(all_TARGETS)
check_TARGETS += $(check_PROGRAMS)

.PHONY: check-targets
check-targets: $(check_TARGETS)

.PHONY: check
check: tests/run-tests beep $(check_TARGETS)
	env PACKAGE_VERSION="${PACKAGE_VERSION}" \
	/bin/bash $< $(<D) $(PWD)/beep

.PHONY: clean
clean:
	rm -f $(bin_PROGRAMS) $(sbin_PROGRAMS) $(check_PROGRAMS)
	rm -f $(CLEANFILES)
	rm -f *.dep
	rm -rf .deps
	rm -f *.lst *.gcc-lst
	rm -f tests/*.new tests/*.actual
	rm -rf dox
	rm -f *.o *.i *.s *.bc

.PHONY: doc
doc: $(doc_DATA)


########################################################################
# install and uninstall targets
########################################################################


DESTDIR =


define make-installdir
$$(DESTDIR)$(1):
	$$(INSTALL) -d -m 0755 $$@
endef

$(foreach dir,$(sort $(foreach d,$(dir-vars),$($(d)))),$(eval $(call make-installdir,$(dir))))


# install-file target-accu-var filemode dirvar filetoinstall
#
# Example:
#   $(eval $(call install-file,installed-files-html,0644,htmldir,html/foobar.html))
define install-file
installed-files += $$(DESTDIR)$$($(2))/$$(notdir $(3))
$$(DESTDIR)$$($(2))/$$(notdir $(3)): $(3) | $$(DESTDIR)$$($(2))
	$$(INSTALL) -p -m $(1) $$< $$@
endef


# install-fileset bin_PROGRAMS [install-subtarget]
#
# Examples:
#   $(eval $(call install-fileset,bin_PROGRAMS))
#   $(eval $(call install-fileset,html_DATA,html))
define install-fileset
ifneq (,$$($(1)))
$$(foreach f,$$($(1)),$$(eval $$(call install-file,$$(if $$(filter PROGRAMS SCRIPTS,$$(lastword $$(subst _, ,$(1)))),0755,0644),$$(firstword $$(subst _, ,$(1)))dir,$$(f))))
endif
endef


$(eval $(call install-fileset,bin_PROGRAMS))
$(eval $(call install-fileset,sbin_PROGRAMS))
$(eval $(call install-fileset,man1_DATA))
$(eval $(call install-fileset,doc_DATA))
$(eval $(call install-fileset,html_DATA))
$(eval $(call install-fileset,contrib_DATA))
$(eval $(call install-fileset,contrib_SCRIPTS))


.PHONY: install
install: all $(installed-files)

.PHONY: uninstall
uninstall:
	rm -f $(installed-files)


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

$(info #=======================================================================)

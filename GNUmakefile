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
# This GNUmakefile makes extensive use of GNU make features and some
# tricks for using GNU make, such as
#   * Advanced Auto Dependency Generation[2]
#   * Deferred Simple Variable Expansion[3]
#
# [1]: https://www.gnu.org/prep/standards/html_node/Makefile-Conventions.html
# [2]: http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
# [3]: http://make.mad-scientist.net/deferred-simple-variable-expansion/
#
########################################################################


########################################################################
# Package metadata
########################################################################

PACKAGE_TARNAME = beep
PACKAGE_VERSION = 1.4.13


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
# versions, or to disable specific parts of the build. For example, to
# test the portability of the SED commands, you can run with
# SED="busybox sed", or to disable building HTML files from the
# markdown files you can run with PANDOC="false".

CMP       = cmp
DIFF      = diff
DOT       = dot
DOXYGEN   = doxygen
EGREP     = $(GREP) -E
FIND      = find
GIT       = git
GREP      = grep
INSTALL   = install
MKDIR_P   = mkdir -p
PANDOC    = pandoc
PYTHON3   = python3
SED       = sed
TAR       = tar
TPUT      = tput
WC        = wc

DIFF_U    = $(DIFF) -u

# The _DATA and _PROGRAM variants are GNU makefile convention.
# The _DIR variant is our idea.
INSTALL_DIR     = $(INSTALL) -m 0755 -d
INSTALL_DATA    = $(INSTALL) -m 0644 -p
INSTALL_PROGRAM = $(INSTALL) -m 0755 -p


########################################################################
# This needs to be the first rule
########################################################################

.PHONY: all
all: all-local


########################################################################
# Initialize some things for the build system
########################################################################


# Prevent make from using its built-in rules
.SUFFIXES:
COMPILE.c = false COMPILE.c
LINK.c    = false LINK.c


########################################################################
# Variables to add to later
########################################################################

dist-files  =
dist-files += GNUmakefile

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
#   $(eval $(call check-cflags,common-CFLAGS,-fasynchronous-unwind-tables))
define check-cflags
$(1) += $$(if $$(shell if $$(CC) $$(patsubst -Wno-%,-W%,$(2)) -x c -o compile-check.o -c - < /dev/null > /dev/null 2>&1; then echo yes; else :; fi; rm -f compile-check.o > /dev/null 2>&1),$(2))
endef

# This might be useful or not.
CFLAGS   :=
CPPFLAGS  =
LDFLAGS   =
LDADD     =

# Flags common to all executable targets
common_CFLAGS   :=
common_CPPFLAGS  =
common_LDFLAGS   =
common_LDADD     =

common_CPPFLAGS += -DPACKAGE_TARNAME='"$(PACKAGE_TARNAME)"'
common_CPPFLAGS += -DPACKAGE_VERSION='"$(PACKAGE_VERSION)"'
common_CFLAGS   += -std=gnu99
common_CFLAGS   += $(if $(filter %.o,$@),-Wa$(comma)-adhlns=$(@:.o=.lst))
common_CFLAGS   += -pedantic
$(eval $(call check-cflags,common_CFLAGS,-Werror=unknown-warning-option))
$(eval $(call check-cflags,common_CFLAGS,-Wall))
$(eval $(call check-cflags,common_CFLAGS,-Wextra))
$(eval $(call check-cflags,common_CFLAGS,-Weverything))
$(eval $(call check-cflags,common_CFLAGS,-Werror))
$(eval $(call check-cflags,common_CFLAGS,-Wno-padded))
$(eval $(call check-cflags,common_CFLAGS,-Werror=format-security))
$(eval $(call check-cflags,common_CFLAGS,-Wno-disabled-macro-expansion))
$(eval $(call check-cflags,common_CFLAGS,-Wno-format-nonliteral))
$(eval $(call check-cflags,CPPFLAGS,-D_FORTIFY_SOURCE=2))
$(eval $(call check-cflags,CPPFLAGS,-D_GLIBCXX_ASSERTIONS))
# $(eval $(call check-cflags,CFLAGS,-Wp$$(comma)-D_FORTIFY_SOURCE=2))
# $(eval $(call check-cflags,CFLAGS,-Wp$$(comma)-D_GLIBCXX_ASSERTIONS))
$(eval $(call check-cflags,CFLAGS,-fasynchronous-unwind-tables))
$(eval $(call check-cflags,CFLAGS,-fanalyzer))
$(eval $(call check-cflags,CFLAGS,-fstack-protector-strong))
$(eval $(call check-cflags,CFLAGS,-fstack-clash-protection))
$(eval $(call check-cflags,CFLAGS,-fcf-protection))
$(eval $(call check-cflags,CFLAGS,-fsanitize=undefined))


CFLAGS += -O2 -g
CFLAGS += -save-temps=obj


# Create this file to override any of the make variables defined
# above.
-include local.mk


ifneq (,$(V))
$(info #=======================================================================)
else
$(info In case of build problems, try running `make' with V=1 to help find the cause.)
endif


ifneq (,$(V))
$(info # common_CFLAGS=$(common_CFLAGS))
$(info # common_CPPFLAGS=$(common_CPPFLAGS))
$(info # common_LDADD=$(common_LDADD))
$(info # common_LDFLAGS=$(common_LDFLAGS))
$(info # CFLAGS=$(CFLAGS))
$(info # CPPFLAGS=$(CPPFLAGS))
$(info # LDADD=$(LDADD))
$(info # LDFLAGS=$(LDFLAGS))
endif


########################################################################
# Detect libraries
########################################################################


########################################################################
# Define executables and their flags
########################################################################

check_PROGRAMS            += issue-6-benchmark
issue_6_benchmark_SOURCES  = issue-6-benchmark.c
issue_6_benchmark_LDADD    = -lm

bin_PROGRAMS += beep
beep_SOURCES  =
beep_SOURCES += beep-compiler.h
beep_SOURCES += beep-types.h
beep_SOURCES += beep-log.c
beep_SOURCES += beep-log.h
beep_SOURCES += beep-main.c
beep_SOURCES += beep-library.c
beep_SOURCES += beep-library.h
beep_SOURCES += beep-usage.c
beep_SOURCES += beep-usage.h
beep_SOURCES += beep-drivers.c
beep_SOURCES += beep-drivers.h

# The drivers here use `__attribute__((constructor))` functions to
# register themselves with `beep_drivers_register()`, so the last one
# listed here will be linked last and have its constructor called at
# the latest time, and thus will have its `driver_detect()` function
# called first.

beep_SOURCES += beep-driver.h
beep_SOURCES += beep-driver-console.c
beep_SOURCES += beep-driver-evdev.c

EXTRA_DIST   += beep-driver-noop.c
ifneq ($(BEEP_DEBUG_BUILD),)
beep_SOURCES += beep-driver-noop.c
endif

beep_LDADD    =

beep-log.o : override common_CPPFLAGS += -D_GNU_SOURCE
# beep-log.clang-o : override CFLAGS_clang += -Wno-format-nonliteral

# sbin_PROGRAMS    += beep-foo
# beep_foo_SOURCES  =
# beep_foo_SOURCES += beep-log.c
# beep_foo_SOURCES += beep.c
# beep_foo_LDADD    =
# beep_foo_LDADD   += -lm


########################################################################
# Built sources
########################################################################

EXTRA_DIST    += beep-usage.txt.in
CLEANFILES    += beep-usage.txt

BUILT_SOURCES += beep-usage.c
CLEANFILES    += beep-usage.c
beep-usage.c: beep-usage.txt
	$(inhibit-build-command)
	printf '%s\n' '/* Auto-generated from `$<`. Modify that file instead. */' > $@
	printf '%s\n' '#include "beep-usage.h"' >> $@
	printf '%s\n' 'char beep_usage[] =' >> $@
	set -e; IFS=""; while read line; do \
		printf '  "%s\\n"\n' "$${line}" >> $@; \
	done < $<
	printf '%s\n' '  ;' >> $@


########################################################################
# Compile and Link rules including automatic dependency generation
#
# For details on the automatic dependency generation, see
# http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
########################################################################


# CALL: define-link-rule <executable> <executable_as_varname_part> <dircomponent>
# Defines the per-executable rules.
define define-link-rule
CLEANFILES += $(1).map
dist-files += $$($(2)_SOURCES)
$(2)_OBJS := $$(foreach src,$$($(2)_SOURCES),$$(if $$(filter %.c,$$(src)),$$(src:%.c=%.o),$$(if $$(filter %.h,$$(src)),,$$(error Unhandled source type in $(2)_SOURCES: $$(src)))))

$(1): $$($(2)_OBJS)
	$$(inhibit-build-command)
	@$$(call print-rule-description,LINK,$$@)
	$$(CC) -Wl,-Map=$(1).map,--cref $$(common_CFLAGS) $$(CFLAGS) $$(common_LDFLAGS) $$($(2)_LDFLAGS) $$(LDFLAGS) -o $$@ $$^ $$(common_LDADD) $$($(2)_LDADD) $$(LDADD)

$$(patsubst %.o,.deps/%.o.dep,$$($(2)_OBJS))):

-include $$(wildcard $$(patsubst %.o,.deps/%.o.dep,$$($(2)_OBJS)))
endef


$(foreach exec,$(bin_PROGRAMS),  $(eval $(call define-link-rule,$(exec),$(subst -,_,$(exec)),bin)))
$(foreach exec,$(check_PROGRAMS),$(eval $(call define-link-rule,$(exec),$(subst -,_,$(exec)),check)))
$(foreach exec,$(sbin_PROGRAMS), $(eval $(call define-link-rule,$(exec),$(subst -,_,$(exec)),sbin)))

%.o: %.c | .deps
	$(inhibit-build-command)
	@$(call print-rule-description,COMPILE,$@)
	$(CC) -MT $@ -MMD -MP -MF .deps/$*.o.dep $(common_CPPFLAGS) $(CPPFLAGS) $(common_CFLAGS) $(CFLAGS) -o $@ -c $<

.deps:
	$(inhibit-build-command)
	@$(MKDIR_P) $@


########################################################################
# Generate doc and similar files
########################################################################

EXTRA_DIST += gen-freq-table

EXTRA_DIST += beep.1.in
man1_DATA  += beep.1
CLEANFILES += beep.1

CLEANFILES       += CREDITS.html
CLEANFILES       += NEWS.html
CLEANFILES       += PERMISSIONS.html
CLEANFILES       += README.html

CLEANFILES       += DEVELOPMENT.html
CLEANFILES       += INSTALL.html
CLEANFILES       += PACKAGING.html

EXTRA_DIST       += pandoc.css

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
	$(inhibit-build-command)
	@$(call print-rule-description,PANDOC,$@)
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

EXTRA_DIST    += beep-config.h.in
CLEANFILES    += beep-config.h
BUILT_SOURCES += beep-config.h
beep-main.o : beep-config.h

EXTRA_DIST += Doxyfile.in
CLEANFILES += Doxyfile
CLEANFILES += Doxyfile.new

%: %.in GNUmakefile
	$(inhibit-build-command)
	@$(call print-rule-description,SUBSTITUTE,$@)
	$(SED) $(REPLACEMENTS) < $< > $@.new
	@if $(EGREP) '@([A-Za-z][A-Za-z0-9_]*)@' $@.new; then \
		echo "Error: GNUmakefile fails to substitute some of the variables in \`$<'."; \
		exit 1; \
	fi
	mv -f $@.new $@

CLEANFILES += doxygen.stamp
.PHONY: doxygen.stamp
doxygen.stamp: Doxyfile $(wildcard *.c) $(wildcard *.h)
	$(inhibit-build-command)
	@$(call print-rule-description,DOXYGEN,html dox)
	$(DOXYGEN) $<
	echo > $@

dox: doxygen.stamp

.PHONY: serve-dox
serve-dox: dox
	@$(call print-rule-description,SERVING,doxygen html files on HTTP server)
	$(PYTHON3) -m http.server --directory dox/html

EXTRA_DIST += COPYING
doc_DATA   += COPYING

EXTRA_DIST += CREDITS.md
doc_DATA   += CREDITS.md

EXTRA_DIST += NEWS.md
doc_DATA   += NEWS.md

EXTRA_DIST += README.md
doc_DATA   += README.md

EXTRA_DIST += PERMISSIONS.md
doc_DATA   += PERMISSIONS.md

EXTRA_DIST += DEVELOPMENT.md
EXTRA_DIST += INSTALL.md
EXTRA_DIST += PACKAGING.md

EXTRA_DIST      += contrib/failure-beeps
contrib_SCRIPTS += contrib/failure-beeps
EXTRA_DIST      += contrib/success-beeps
contrib_SCRIPTS += contrib/success-beeps
EXTRA_DIST      += contrib/morse/morse2beep.pl
contrib_SCRIPTS += contrib/morse/morse2beep.pl
EXTRA_DIST      += contrib/morse/morse2beep.sed
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
	$(inhibit-build-command)
	@$(call print-rule-description,CHECK,$(PWD)/beep)
	env PACKAGE_VERSION="${PACKAGE_VERSION}" \
	/bin/bash $< $(<D) $(PWD)/beep

dist-files += $(wildcard tests/README.md)
dist-files += $(wildcard tests/run-tests)
dist-files += $(wildcard tests/*.bash)
dist-files += $(wildcard tests/*.sh)
dist-files += $(wildcard tests/*.expected.[0-9])

EXTRA_DIST += testbuild-all

EXTRA_DIST += .gitignore
EXTRA_DIST += .github/workflows/beep-build.yml

.PHONY: clean
clean:
	@$(call print-rule-description,CLEANUP,all built files)
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


define define-install-dir-rule
$$(DESTDIR)$(1):
	@$$(call print-rule-description,INSTALL,$$@/)
	$$(INSTALL_DIR) $$@
endef

dir-vars = $(sort $(foreach varname,$(sort $(.VARIABLES)),$(if $(filter %dir,$(varname)),$(varname))))

$(foreach dir,$(sort $(foreach d,$(dir-vars),$($(d)))),$(eval $(call define-install-dir-rule,$(dir))))


# define-install-file-rule target-accu-var installvar dirvar filetoinstall
#
# Example:
#   $(eval $(call define-install-file-rule,INSTALL_DATA,htmldir,html/foobar.html))
define define-install-file-rule
installed-files += $$(DESTDIR)$$($(2))/$$(notdir $(3))
$$(DESTDIR)$$($(2))/$$(notdir $(3)): $(3) | $$(DESTDIR)$$($(2))
	@$$(call print-rule-description,INSTALL,$$@)
	$$($(1)) $$< $$@
endef


# define-install-fileset-rules bin_PROGRAMS [install-subtarget]
#
# Examples:
#   $(eval $(call define-install-fileset-rules,bin_PROGRAMS))
#   $(eval $(call define-install-fileset-rules,html_DATA,html))
define define-install-fileset-rules
ifneq (,$$($(1)))
$$(foreach f,$$($(1)),$$(eval $$(call define-install-file-rule,$$(if $$(filter PROGRAMS SCRIPTS,$$(lastword $$(subst _, ,$(1)))),INSTALL_PROGRAM,INSTALL_DATA),$$(firstword $$(subst _, ,$(1)))dir,$$(f))))
endif
endef


$(eval $(call define-install-fileset-rules,bin_PROGRAMS))
$(eval $(call define-install-fileset-rules,sbin_PROGRAMS))
$(eval $(call define-install-fileset-rules,man1_DATA))
$(eval $(call define-install-fileset-rules,doc_DATA))
$(eval $(call define-install-fileset-rules,html_DATA))
$(eval $(call define-install-fileset-rules,contrib_DATA))
$(eval $(call define-install-fileset-rules,contrib_SCRIPTS))


.PHONY: install
install: all $(installed-files)


# This is for use with the "install-nobuild" target.
#
# For every recipe which creates/modifies a file in the build tree,
# use $(inhibit-build-command) as the first command.
#
# The default value of @: produces no output and succeeds, so that the
# further recipe lines can actually do the work.
#
# If you want to disable build, set this to "false" or "false
# 'comment'", so that each invoked build rule will fail instantly
# before doing any actual work.
inhibit-build-command = @:

.PHONY: install-nobuild
install-nobuild: install
install-nobuild : inhibit-build-command=@printf "Error: 'make install-nobuild' inhibits all build rules.\n       Try running 'make' to build $@ first, or run\n       'make install' which implicitly builds $@ first.\n"; false

.PHONY: uninstall
uninstall:
	@$(call print-rule-description,UNINSTALL,all installed files)
	rm -f $(installed-files)


########################################################################
# "make dist" related rules and definitions
########################################################################

dist-files += $(EXTRA_DIST)

sorted-dist-files := $(filter-out $(BUILT_SOURCES),$(sort $(dist-files)))

distdir = $(PACKAGE_TARNAME)-$(PACKAGE_VERSION)

.PHONY: dist
dist: $(distdir).tar.gz

TAR_VERBOSE = --verbose --show-transformed-names
TAR_VERBOSE =
$(distdir).tar.gz: $(sorted-dist-files)
	$(inhibit-build-command)
	@$(call print-rule-description,DIST TARBALL,$@)
	@$(TAR) --transform='s|^|$(distdir)/|' --auto-compress --create --file=$@ $(TAR_VERBOSE) $(sorted-dist-files)

.PHONY: distcheck
distcheck: dist-check-install-uninstall

# Check that this distribution satisfies a few conditions, such as
#   * After "make" and "make clean", the source tree is the same.
#   * After "make install" and "make uninstall", there are no files
#     left installed.
#   * After "make", "make install" does not need to build anything,
#     i.e. "make install-nobuild" succeeds after "make".
.PHONY: dist-check-install-uninstall
dist-check-install-uninstall: $(distdir).tar.gz
	@$(call print-rule-description,DIST CHECK,all and clean, install and uninstall)
	rm -rf __tmp
	mkdir __tmp
	cd __tmp && $(TAR) xf ../$(distdir).tar.gz
	echo "prefix = ${PWD}/__tmp/_prefix" > __tmp/$(distdir)/local.mk
	(cd __tmp/$(distdir) && $(FIND) . -type f) | env LC_ALL=C sort > __tmp/before-build.filelist
	$(MAKE) -C __tmp/$(distdir)
	$(MAKE) -C __tmp/$(distdir) check
	$(MAKE) -C __tmp/$(distdir) install-nobuild
	(cd __tmp/_prefix && $(FIND) . -type f) | env LC_ALL=C sort > __tmp/after-install.filelist
	@n="$$($(WC) -l < __tmp/after-install.filelist)"; \
	if test "$$n" -eq 0; then \
	  echo "Error: Found no installed files"; \
	  exit 1; \
	elif test "$$n" -gt 10; then \
	  echo "Found $${n} installed files: good."; \
	else \
	  echo "Error: Found only $${n} installed files"; \
	  exit 1; \
	fi
	$(MAKE) -C __tmp/$(distdir) uninstall
	(cd __tmp/_prefix && $(FIND) . -type f) | env LC_ALL=C sort > __tmp/after-uninstall.filelist
	@n="$$($(WC) -l < __tmp/after-uninstall.filelist)"; \
	if test "$$n" -gt 0; then \
	  echo "Error: Found $${n} left over installed files after uninstall"; \
	  exit 1; \
	else \
	  echo "Found no installed files after uninstall"; \
	fi
	$(MAKE) -C __tmp/$(distdir) clean
	(cd __tmp/$(distdir) && $(FIND) . -type f) | env LC_ALL=C sort > __tmp/after-clean.filelist
	cd __tmp && $(DIFF_U) before-build.filelist after-clean.filelist


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
PACKAGE_TARBASE = $(eval PACKAGE_TARBASE := $$(PACKAGE_TARNAME)-$$(shell $$(GIT) describe --tags | $$(SED) 's/^v\([0-9]\)/\1/'))$(PACKAGE_TARBASE)
.PHONY: git-dist
git-dist:
	@$(call print-rule-description,GIT ARCHIVE,$(PACKAGE_TARBASE).tar.gz)
	$(GIT) archive --format=tar.gz --prefix=$(PACKAGE_TARBASE)/ --output=$(PACKAGE_TARBASE).tar.gz HEAD

# Check that the lists of files inside the "git archive" and the "tar"
# dist tarballs are the same.
.PHONY: compare-tarballs
distcheck: compare-tarballs
compare-tarballs: dist git-dist
	@$(call print-rule-description,COMPARE,contents of git archive and dist tarball)
	rm -rf tarball-dist tarball-git-dist
	mkdir tarball-dist     && cd tarball-dist     && $(TAR) xf ../$(distdir).tar.gz
	mkdir tarball-git-dist && cd tarball-git-dist && $(TAR) xf ../$(PACKAGE_TARBASE).tar.gz
	diff -ruN tarball-git-dist/$(PACKAGE_TARBASE) tarball-dist/$(distdir)
	rm -rf tarball-dist tarball-git-dist

endif
endif


########################################################################
# Print rule descriptions and silent rules
#
# This is a bit more complex than mad scientist's simple silent
# rules[1]:
#
#   * In silent mode, we print aligned descriptions for each target
#     being built.
#
#   * In non-silent mode, we print a description of the recipe before
#     each recipe is actually run, but highlighted a bit to stand out
#     from the endless sequence of recipe command text: The description
#     line always starts with a "#", and on a TTY, it will be printed
#     in bold.
#
# [1]: http://make.mad-scientist.net/managing-recipe-echoing/
########################################################################

# Set V to empty to disable silent rules, non-empty to enable them.
V=1
V=

# Note that we cannot use "test -t 1" inside a $(shell ) command, as
# the $(shell) will capture stdout into a variable, so the "test -t 1"
# will always test negative.

ifeq (,$(V))
.SILENT:
print-rule-description = printf "%12s %s\n" "$(1)" "$(2)"
else
print-rule-description = if test -t 1; then cb="$$($(TPUT) bold)"; cn="$$($(TPUT) sgr0)"; fi; printf "%s\# %s %s%s\n" "$$cb" "$(1)" "$(2)" "$$cn"
endif


########################################################################
# End of GNUmakefile
########################################################################

ifneq (,$(V))
$(info #=======================================================================)
endif

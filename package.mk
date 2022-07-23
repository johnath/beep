contribdir = $(docdir)/contrib

ACLOCAL_FLAGS   += -I m4

EXTRA_DIST      += m4/sb-prog.m4
aclocal_m4_deps += m4/sb-prog.m4
EXTRA_DIST      += m4/sb-summary.m4
aclocal_m4_deps += m4/sb-summary.m4

EXTRA_DIST  += COPYING
doc_DATA    += COPYING

EXTRA_DIST  += CREDITS.md
doc_DATA    += CREDITS.md

EXTRA_DIST  += NEWS.md
doc_DATA    += NEWS.md

EXTRA_DIST  += README.md
doc_DATA    += README.md

EXTRA_DIST  += PERMISSIONS.md
doc_DATA    += PERMISSIONS.md

EXTRA_DIST  += DEVELOPMENT.md
noinst_DATA += DEVELOPMENT.md

EXTRA_DIST  += INSTALL.md
noinst_DATA += INSTALL.md

EXTRA_DIST  += PACKAGING.md
noinst_DATA += PACKAGING.md

EXTRA_DIST  += pandoc.css

ifeq (yes,$(shell if $(PANDOC) --version > /dev/null 2>&1; then echo yes; else echo no; fi))

html_DATA   += pandoc.css

CLEANFILES  += CREDITS.html
html_DATA   += CREDITS.html

CLEANFILES  += NEWS.html
html_DATA   += NEWS.html

CLEANFILES  += PERMISSIONS.html
html_DATA   += PERMISSIONS.html

CLEANFILES  += README.html
html_DATA   += README.html

CLEANFILES  += DEVELOPMENT.html
noinst_DATA += DEVELOPMENT.html

CLEANFILES  += INSTALL.html
noinst_DATA += INSTALL.html

CLEANFILES  += PACKAGING.html
noinst_DATA += PACKAGING.html

%.html: %.md
	$(inhibit-build-command)
	@$(call print-rule-description,PANDOC,$(<F),TO,$@)
	title="$$($(SED) -n 1p $<)"; \
	if test "x$$title" = "x"; then title="$(<F)"; fi; \
	$(PANDOC) --from gfm --to html --standalone -M pagetitle="$$title" -M title="" -c pandoc.css $< -o $@
endif

EXTRA_DIST      += contrib/failure-beeps
contrib_SCRIPTS += contrib/failure-beeps

EXTRA_DIST      += contrib/success-beeps
contrib_SCRIPTS += contrib/success-beeps

EXTRA_DIST      += contrib/morse/morse2beep.pl
contrib_SCRIPTS += contrib/morse/morse2beep.pl

EXTRA_DIST      += contrib/morse/morse2beep.sed
contrib_SCRIPTS += contrib/morse/morse2beep.sed

check_PROGRAMS            += issue-6-benchmark
issue_6_benchmark_SOURCES += issue-6-benchmark.c
issue_6_benchmark_LDADD   += -lm

bin_PROGRAMS  += beep
beep_SOURCES  += beep-compiler.h
beep_SOURCES  += beep-config.h
beep_SOURCES  += beep-driver.h
beep_SOURCES  += beep-drivers.c
beep_SOURCES  += beep-drivers.h
beep_SOURCES  += beep-library.c
beep_SOURCES  += beep-library.h

beep_SOURCES  += beep-log.c
beep_SOURCES  += beep-log.h
.objs/beep/beep-log.o : override CPPFLAGS += -D_GNU_SOURCE

beep_SOURCES  += beep-main.c
beep_SOURCES  += beep-types.h

# The drivers here use `__attribute__((constructor))` functions to
# register themselves with `beep_drivers_register()`, so the last one
# listed here will be linked last and have its constructor called at
# the latest time, and thus will have its `driver_detect()` function
# called first.
beep_SOURCES  += beep-driver-console.c
beep_SOURCES  += beep-driver-evdev.c
EXTRA_DIST    += beep-driver-noop.c
#beep_SOURCES  += beep-driver-noop.c

EXTRA_DIST    += beep-usage.txt.in
CLEANFILES    += beep-usage.txt
CLEANFILES    += beep-usage.c
BUILT_SOURCES += beep-usage.c
beep_SOURCES  += beep-usage.c
beep_SOURCES  += beep-usage.h

beep-usage.c: beep-usage.txt
	$(inhibit-build-command)
	@$(call print-rule-description,CONVERT,$<,TO,$@)
	( set -e; \
	  printf '%s\n' '/* Auto-generated from `$<`. Modify that file instead. */'; \
	  printf '%s\n' '#include "beep-usage.h"'; \
	  printf '%s\n' 'char beep_usage[] ='; \
	  IFS=""; while read line; do \
	    printf '  "%s\\n"\n' "$${line}"; \
	  done < $<; \
	  printf '%s\n' '  ;'\
	) > $@

DEFAULT_FREQ   = 440
DEFAULT_LENGTH = 200
DEFAULT_DELAY  = 100

$(foreach var,FREQ LENGTH DELAY,$(eval $(call define-sed-substitution,DEFAULT_$(var))))

EXTRA_DIST    += beep-config.h.in
CLEANFILES    += beep-config.h
BUILT_SOURCES += beep-config.h
.objs/beep/beep-main.o : beep-config.h

EXTRA_DIST += beep.1.in
CLEANFILES += beep.1
man1_MANS  += beep.1

EXTRA_DIST += gen-freq-table

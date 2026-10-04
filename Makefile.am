AUTOMAKE_OPTIONS = foreign
AM_CFLAGS = -std=c11 -Wall -Wextra -pedantic
bin_PROGRAMS = xyzzy
xyzzy_SOURCES = xyzzy.c
EXTRA_DIST = LICENSE README.md man/xyzzy.1 homebrew/xyzzy.rb scripts/packager.sh spec/prd.md

install-data-local:
	$(MKDIR_P) "$(DESTDIR)$(docdir)"
	$(INSTALL_DATA) "$(srcdir)/README.md" "$(DESTDIR)$(docdir)/README.md"
	$(INSTALL_DATA) "$(srcdir)/LICENSE" "$(DESTDIR)$(docdir)/LICENSE"
	$(MKDIR_P) "$(DESTDIR)$(mandir)/man1"
	$(INSTALL_DATA) "$(srcdir)/man/xyzzy.1" "$(DESTDIR)$(mandir)/man1/xyzzy.1"

uninstall-local:
	rm -f "$(DESTDIR)$(docdir)/LICENSE"
	rm -f "$(DESTDIR)$(docdir)/README.md"
	rm -f "$(DESTDIR)$(mandir)/man1/xyzzy.1"

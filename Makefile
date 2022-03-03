## Configuration
DESTDIR    =
PREFIX     =/usr/local
AR         =ar
CC         =gcc
CFLAGS     =-Wall -g
CPPFLAGS   =
LIBS       ="-l:libmdb.a" "-l:libgdbm.a" "-l:libuuid.a"
## Sources and targets
PROGRAMS   =miniauth
LIBRARIES  =libminiauth.a
HEADERS    =miniauth.h
MARKDOWNS  =
MANPAGES_3 =miniauth.3.md
SOURCES    =
## AUXILIARY
CFLAGS_ALL =$(LDFLAGS) $(CFLAGS) $(CPPFLAGS)

## STANDARD TARGETS
all: $(PROGRAMS) $(LIBRARIES)
help:
	@echo "all     : Build everything."
	@echo "clean   : Clean files."
	@echo "install : Install all produced files."
install: all
	install -d                  $(DESTDIR)$(PREFIX)/bin
	install -m755 $(PROGRAMS)   $(DESTDIR)$(PREFIX)/bin
	install -d                  $(DESTDIR)$(PREFIX)/lib
	install -m644 $(LIBRARIES)  $(DESTDIR)$(PREFIX)/lib
	install -d                  $(DESTDIR)$(PREFIX)/include
	install -m644 $(HEADERS)    $(DESTDIR)$(PREFIX)/include
	install -d                  $(DESTDIR)$(PREFIX)/share/man/man3
	install -m644 $(MANPAGES_3) $(DESTDIR)$(PREFIX)/share/man/man3
clean:
	rm -f $(PROGRAMS) $(LIBRARIES)

## LIBRARY
libminiauth.a : miniauth.c $(HEADERS)
	$(CC) -c miniauth.c $(CFLAGS_ALL)
	$(AR) -crs $@ miniauth.o
	rm -f miniauth.o
miniauth: main.c libminiauth.a $(HEADERS)
	$(CC) -o $@ main.c libminiauth.a $(LIBS)

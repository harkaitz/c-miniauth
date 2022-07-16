DESTDIR    =
PREFIX     =/usr/local
AR         =ar
CC         =gcc
CFLAGS     =-Wall -g
PROGRAMS   =miniauth$(EXE)
LIBRARIES  =libminiauth.a
HEADERS    =miniauth.h
LIBS       ="-l:libmdb.a" "-l:libhiredis.a" "-l:libuuid.a"
CFLAGS_ALL =$(LDFLAGS) $(CFLAGS) $(CPPFLAGS)

##
all: $(PROGRAMS) $(LIBRARIES)
install: all
	install -d                  $(DESTDIR)$(PREFIX)/bin
	install -m755 $(PROGRAMS)   $(DESTDIR)$(PREFIX)/bin
	install -d                  $(DESTDIR)$(PREFIX)/lib
	install -m644 $(LIBRARIES)  $(DESTDIR)$(PREFIX)/lib
	install -d                  $(DESTDIR)$(PREFIX)/include
	install -m644 $(HEADERS)    $(DESTDIR)$(PREFIX)/include
clean:
	rm -f $(PROGRAMS) $(LIBRARIES)

##
libminiauth.a : miniauth.c $(HEADERS)
	$(CC) -c miniauth.c $(CFLAGS_ALL)
	$(AR) -crs $@ miniauth.o
	rm -f miniauth.o
miniauth$(EXE): main.c libminiauth.a $(HEADERS)
	$(CC) -o $@ main.c libminiauth.a $(LIBS)
## -- manpages --
ifneq ($(PREFIX),)
MAN_3=./miniauth.3 
install: install-man3
install-man3: $(MAN_3)
	mkdir -p $(DESTDIR)$(PREFIX)/share/man/man3
	cp $(MAN_3) $(DESTDIR)$(PREFIX)/share/man/man3
endif
## -- manpages --
## -- license --
ifneq ($(PREFIX),)
install: install-license
install-license: LICENSE
	mkdir -p $(DESTDIR)$(PREFIX)/share/doc/c-miniauth
	cp LICENSE $(DESTDIR)$(PREFIX)/share/doc/c-miniauth
endif
## -- license --

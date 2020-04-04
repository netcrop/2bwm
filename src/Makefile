VERSION=0.2

RM=/bin/rm
PREFIX?=/usr/local
LIB_SUFFIX?=lib
MANPREFIX?=$(PREFIX)/share/man
TWOBWM_PATH?=${PREFIX}/bin/2bwm
X11_INCLUDE?=/usr/local/include

#CC=clang
DIST=2bwm-$(VERSION)
SRC=2bwm.c
DISTFILES=Makefile README.md TODO 2bwm.1 $(SRC)
DEBUG = -ggdb3 -g3
CFLAGS+= -Os -s -I${X11_INCLUDE} \
		-DTWOBWM_PATH=\"${TWOBWM_PATH}\" 

LDFLAGS+=-L${PREFIX}/${LIB_SUFFIX} -lxcb -lxcb-randr -lxcb-keysyms \
		 -lxcb-icccm -lxcb-ewmh -lxcb-xrm
TARGETS=2bwm 
OBJS=2bwm.o

2bwm: $(OBJS)
	$(CC) -Wall -Werror -Wextra -Wuninitialized -o $@ $(OBJS) $(CFLAGS) $(LDFLAGS)

2bwm.o: 2bwm.c Makefile

install: $(TARGETS)
	test -d $(DESTDIR)$(PREFIX)/bin || mkdir -p $(DESTDIR)$(PREFIX)/bin
	install -pm 755 2bwm $(DESTDIR)$(PREFIX)/bin
	test -d $(DESTDIR)$(MANPREFIX)/man1 || mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	install -pm 644 2bwm.1 $(DESTDIR)$(MANPREFIX)/man1/2bwm.1

uninstall: deinstall
deinstall:
	$(RM) $(DESTDIR)$(PREFIX)/bin/2bwm
	$(RM) $(DESTDIR)$(MANPREFIX)/man1/2bwm.1

$(DIST).tar.bz2:
	mkdir $(DIST)
	cp $(DISTFILES) $(DIST)/
	tar cf $(DIST).tar --exclude .git $(DIST)
	bzip2 -9 $(DIST).tar
	$(RM) -rf $(DIST)

dist: $(DIST).tar.bz2

clean:
	$(RM) -f $(TARGETS) *.o

distclean: clean
	$(RM) -f $(DIST).tar.bz2

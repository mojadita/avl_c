# $Id: Makefile,v 1.5 2014/08/08 19:25:50 luis Exp $
# Author: Luis Colorado <lc@luiscoloradosistemas.com>
# Date: jue nov 21 23:48:42 CET 2013
# Disclaimer: (C) 2013 LUIS COLORADO SISTEMAS S.L.U.
#       All rights reserved.

PACKAGE         ?= avl_c
MAJOR           ?= 3.4
MINOR           ?= 2
VERSION         ?= "$(MAJOR).$(MINOR)"
CFLAGS          += -DAVL_VERSION=\"$(VERSION)\"
USE_CRC         ?= 0

OS              != uname -o

prefix          ?= /usr/local
execprefix      ?= $(prefix)
bindir          ?= $(prefix)/bin
libdir          ?= $(prefix)/lib
includedir      ?= $(prefix)/include
datarootdir     ?= $(prefix)/share
mandir          ?= $(datarootdir)/man
man3dir         ?= $(mandir)/man3

usr-FreeBSD     ?= root
grp-FreeBSD     ?= wheel
usr-GNU/Linux   ?= bin
grp-GNU/Linux   ?= bin

usr             ?= $(usr-$(OS))
grp             ?= $(grp-$(OS))

RM              ?= -rm -f
LINK            ?= ln -sf
RANLIB          ?= ranlib
GZIP            ?= gzip -v
INSTALL         ?= install

dmod            ?= -m 0755 -d
lmod            ?= -m 0755
xmod            ?= -m 0711
fmod            ?= -m 0644
umod            ?= -o $(usr)
gmod            ?= -g $(grp)
IFLAGS          ?= $(umod) $(gmod)

avl_base        = lib$(PACKAGE)
avl_so          = $(avl_base).so
avl_soname      = $(avl_so).$(MAJOR)
avl_fullname    = $(avl_soname).$(MINOR)
avl_a           = $(avl_base).a
avl_a_objs      = avl.o intavl.o stravl.o
avl_a_objs-$(USE_CRC) = crc.o crc32ieee8023.o
CFLAGS-$(USE_CRC) = -DUSE_CRC=1
avl_a_objs      += $(avl_a_objs-1)
CFLAGS          += $(CFLAGS-1)
avl_so_objs      = $(avl_a_objs:.o=.pico)
toclean         += $(avl_so) $(avl_soname) $(avl_fullname) $(avl_a)
toclean         += $(avl_a_objs) $(avl_so_objs)

targets =tstavl tstavl2 tstavl3
toclean         += $(targets)
ut_libs =-lgmock -lgmock_main -lgtest -lpthread

.PHONY: all clean ut
.SUFFIXES: .pico .3.gz .3

.c.pico:
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

.3.3.gz:
	$(GZIP) < $< > $@

all: $(targets)
ut: $(ut_targets)
test: $(test_targets)
clean:
	$(RM) $(toclean)
depend: *.c
	mkdep *.c

toinstall = $(includedir)/avl.h $(includedir)/avlP.h \
        $(includedir)/intavl.h $(includedir)/stravl.h \
        $(man3dir)/avl.3.gz $(libdir)/$(avl_a) \
		$(libdir)/$(avl_fullname) $(libdir)/$(avl_so) \
		$(libdir)/$(avl_soname)

install: $(toinstall)

$(includedir) $(libdir) $(man3dir):
	$(INSTALL) $(IFLAGS) $(dmod) $@

$(includedir)/avl.h: avl.h
	$(INSTALL) $(IFLAGS) $(fmod) $? $@

$(includedir)/avlP.h: avlP.h
	$(INSTALL) $(IFLAGS) $(fmod) $? $@

$(includedir)/intavl.h: intavl.h
	$(INSTALL) $(IFLAGS) $(fmod) $? $@

$(includedir)/stravl.h: stravl.h
	$(INSTALL) $(IFLAGS) $(fmod) $? $@

$(man3dir)/avl.3.gz: avl.3.gz
	$(INSTALL) $(IFLAGS) $(fmod) $? $@

$(libdir)/$(avl_fullname): $(avl_fullname)
	$(INSTALL) $(IFLAGS) $(lmod) $? $@

$(libdir)/$(avl_a): $(avl_a)
	$(INSTALL) $(IFLAGS) $(fmod) $(avl_a) $@

$(libdir)/$(avl_so):
	$(LINK) $(avl_soname) $@

$(libdir)/$(avl_soname):
	$(LINK) $(avl_fullname) $@

deinstall uninstall:
	$(RM) $(toinstall)

$(avl_fullname): $(avl_so_objs)
	$(LD) $(LDFLAGS) -o $@ -shared -soname=$(avl_soname) $(avl_so_objs)

$(avl_a): $(avl_a_objs:.o=.c)
	$(CC) $(CFLAGS) -c $?
	$(AR) -r $@ $(?:.c=.o)
	$(RM) $(?:.c=.o)
	$(RANLIB) $@

common_objs     = fprintbuf.o
toclean 	   += $(common_objs)

tstavl_objs     = tstavl.o
toclean		   += $(tstavl_objs)
tstavl_deps     = $(avl_a)
tstavl_ldflags  = -L$(libdir) -L.
tstavl_libs     = -l$(PACKAGE)

tstavl: $(tstavl_objs) $(common_objs) $(tstavl_deps)
	$(CC) $(LDFLAGS) -o $@ $($@_objs) $(common_objs) $(tstavl_ldflags) $(tstavl_libs)

tstavl2_objs = tstavl2.o
toclean		+= $(tstavl2_objs)
tstavl2: $(tstavl2_objs) $(common_objs) $(tstavl_deps)
	$(CC) $(LDFLAGS) -o $@ $($@_objs) $(common_objs) $(tstavl_ldflags) $(tstavl_libs)

tstavl3_objs = tstavl3.o
toclean		+= $(tstavl3_objs)
tstavl3: $(tstavl3_objs) $(tstavl_deps)
	$(CC) $(LDFLAGS) -o $@ $($@_objs) $(tstavl_ldflags) $(tstavl_libs)

$(avl_so_objs) $(avl_a_objs): avl.h avlP.h
tstavl.o tstavl2.o tstavl3.o: avl.h
tstavl3.o avl.o: avlP.h
avl.o stravl.o intavl.o tstavl.o tstavl2.o tstavl3.o: avl.h
avl.pico stravl.pico intavl.pico: avl.h
avl.pico: avlP.h

ut_avl_objs = ut_avl.o $(avl_so)

ut_avl: $(ut_avl_objs)
	$(CXX) $(LDFLAGS) -o $@ $(ut_avl_objs) $(ut_libs)

# $Id: Makefile,v 1.5 2014/08/08 19:25:50 luis Exp $

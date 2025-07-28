# $Id: Makefile,v 1.5 2014/08/08 19:25:50 luis Exp $
# Author: Luis Colorado <lc@luiscoloradosistemas.com>
# Date: jue nov 21 23:48:42 CET 2013
# Disclaimer: (C) 2013 LUIS COLORADO SISTEMAS S.L.U.
#       All rights reserved.

PACKAGE         ?=avl_c
MAJOR           ?=3.4
MINOR           ?=2
VERSION         ?="$(MAJOR).$(MINOR)"
CFLAGS          +=-DAVL_VERSION=\"$(VERSION)\"
USE_CRC         ?= 0

prefix          ?=  /usr/local

usr             ?= bin
grp             ?= bin

RM              ?= -rm -f
LINK            ?= ln -sf
RANLIB          ?= ranlib
GZIP            ?= gzip -v
INSTALL         ?= install

idir            ?= $(prefix)/include
ldir            ?= $(prefix)/lib
mdir            ?= $(prefix)/share/man
m3dir           ?= $(mdir)/man3
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

toinstall = $(idir)/avl.h $(idir)/avlP.h $(idir)/intavl.h $(idir)/stravl.h \
			$(m3dir)/avl.3.gz $(ldir)/$(avl_a) $(ldir)/$(avl_fullname) \
			$(ldir)/$(avl_so) $(ldir)/$(avl_soname)

install: $(toinstall)

$(idir) $(ldir) $(m3dir):
	$(INSTALL) $(IFLAGS) $(dmod) $@

$(idir)/avl.h $(idir)/avlP.h $(idir)/intavl.h $(idir)/stravl.h: $(@:T) $(idir)
	$(INSTALL) $(IFLAGS) $(fmod) $(@:T) $(idir)

$(m3dir)/avl.3.gz: avl.3.gz $(m3dir)
	$(INSTALL) $(IFLAGS) $(fmod) $(@:T) $(m3dir)

$(ldir)/$(avl_fullname): $(avl_fullname) $(ldir)
	$(INSTALL) $(IFLAGS) $(lmod) $(avl_fullname) $(ldir)

$(ldir)/$(avl_a): $(avl_a) $(ldir)
	$(INSTALL) $(IFLAGS) $(fmod) $(avl_a) $(ldir)

$(ldir)/$(avl_so): $(ldir)
	$(LINK) $(avl_soname) $@

$(ldir)/$(avl_soname): $(ldir)
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

common_objs = fprintbuf.o
toclean 	+= $(common_objs)

tstavl_objs = tstavl.o
toclean		+= $(tstavl_objs)
tstavl_deps = $(avl_so) $(avl_a)
tstavl_ldflags = -L.
tstavl_libs = -l$(PACKAGE)

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

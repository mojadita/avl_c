# $Id: Makefile,v 1.5 2014/08/08 19:25:50 luis Exp $
# Author: Luis Colorado <lc@luiscoloradosistemas.com>
# Date: jue nov 21 23:48:42 CET 2013
# Disclaimer: (C) 2013 LUIS COLORADO SISTEMAS S.L.U.
# 		All rights reserved.

PACKAGE			?=avl
MAJOR			?=3.2
MINOR			?=7
VERSION			?="$(MAJOR).$(MINOR)"
CFLAGS 			+=-DAVL_VERSION=\"$(VERSION)\"

prefix			?=	${HOME}
user			!=	id -u
group			!=	id -g

usr				?= $(user)
grp				?= $(group)

RM				?= -rm -f
LINK			?= ln -sf
RANLIB			?= ranlib
INSTALL			?= install
idir			?= $(prefix)/include
ldir			?= $(prefix)/lib
dmod			?= -m 0755 -d
xmod			?= -m 0711
fmod			?= -m 0644
umod			?= -o $(usr)
gmod			?= -g $(grp)
IFLAGS			?= $(umod) $(gmod)

avl_base		=lib$(PACKAGE)
avl_so			=$(avl_base).so
avl_soname		=$(avl_so).$(MAJOR)
avl_fullname	=$(avl_soname).$(MINOR)
avl_a			=$(avl_base).a
avl_a_objs		=avl.o crc.o crc32ieee8023.o intavl.o stravl.o
avl_so_objs		=$(avl_a_objs:.o=.pico)
toclean			+=$(avl_so) $(avl_soname) $(avl_fullname) $(avl_a)
toclean			+=$(avl_a_objs) $(avl_so_objs)

targets =tstavl tstavl2 tstavl3
ut_libs =-lgmock -lgmock_main -lgtest -lpthread

.PHONY: all clean ut
.SUFFIXES: .pico

.c.pico:
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

all: $(targets)
ut: $(ut_targets)
clean:
	$(RM) $(toclean)
install: $(targets)
	$(INSTALL) $(IFLAGS) $(dmod) $(idir)
	$(INSTALL) $(IFLAGS) $(dmod) $(ldir)
	$(INSTALL) $(IFLAGS) $(fmod) $(avl_a) $(ldir)
	$(INSTALL) $(IFLAGS) $(xmod) $(avl_fullname) $(ldir)
	$(INSTALL) $(IFLAGS) $(fmod) avl.h $(idir)
	$(LINK) $(avl_soname) $(ldir)/$(avl_so)
	$(LINK) $(avl_fullname) $(ldir)/$(avl_soname)
deinstall:
	$(RM) $(idir)/avl.h
	$(RM) $(ldir)/$(avl_fullname)
	$(RM) $(ldir)/$(avl_soname)
	$(RM) $(ldir)/$(avl_so)
	$(RM) $(ldir)/$(avl_a)

$(avl_so): $(avl_soname)
	$(LINK) $< $@

$(avl_soname): $(avl_fullname)
	$(LINK) $< $@

$(avl_fullname): $(avl_so_objs)
	$(LD) $(LDFLAGS) -o $@ -shared -soname=$(avl_soname) $(avl_so_objs)

$(avl_a): $(avl_a)($(avl_a_objs))
	$(RANLIB) $@

$(avl_a)($(avl_a_objs)): $(avl_a_objs:.o=.c)
	$(CC) $(CFLAGS) -c $?
	$(AR) -r $@ $(?:.c=.o)

	$(RM) $(?:.c=.o)

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

tstavl3.o avl.o: avlP.h
avl.o stravl.o intavl.o tstavl.o tstavl2.o tstavl3.o: avl.h
avl.pico stravl.pico intavl.pico: avl.h
avl.pico: avlP.h crc.h crc32ieee8023.h
crc.o crc.pico: crc.h
crc32ieee8023.o crc32ieee8023.pico: crc32ieee8023.h

ut_avl_objs = ut_avl.o libavl.so

ut_avl: $(ut_avl_objs)
	$(CXX) $(LDFLAGS) -o $@ $(ut_avl_objs) $(ut_libs)

# $Id: Makefile,v 1.5 2014/08/08 19:25:50 luis Exp $

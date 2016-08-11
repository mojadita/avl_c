# $Id: Makefile,v 1.5 2014/08/08 19:25:50 luis Exp $
# Author: Luis Colorado <lc@luiscoloradosistemas.com>
# Date: jue nov 21 23:48:42 CET 2013
# Disclaimer: (C) 2013 LUIS COLORADO SISTEMAS S.L.U.
# 		All rights reserved.

package ?= avl
prefix	?= ${HOME}
incdir  ?= $(prefix)/include
bindir  ?= $(prefix)/bin
libdir  ?= $(prefix)/lib
datadir ?= $(prefix)/share/$(package)

MAJOR	=3.2
MINOR	=5
VERSION	= $(MAJOR).$(MINOR)
CFLAGS  ?= -O3
LDFLAGS ?=
CFLAGS	+= -DAVL_VERSION=\"$(VERSION)\" 

INSTALL = install
RM		= rm -f
LINK	= ln -sf
DMOD	?= -m 0755
XMOD	?= -m 0711
LMOD	?= -m 0644
FMOD	?= -m 0644
OWNER	?= -o `id -u` -g `id -g`

avl				?= lib$(package)
avl_so			?= $(avl).so
avl_a			?= $(avl).a
avl_soname		?= $(avl_so).$(MAJOR)
avl_fullname	?= $(avl_soname).$(MINOR)
toclean			+= $(avl_so) $(avl_a) $(avl_soname) $(avl_fullname)

avl_a_objs		= avl.o stravl.o intavl.o
avl_so_objs		= $(avl_a_objs:.o=.so)
toclean			+= $(avl_a_objs) $(avl_so_objs)

targets			= $(avl_so) $(avl_a)
toclean			+= $(targets)

test_targets	= tstavl tstavl2 tstavl3
toclean			+= $(test_targets)

install_headers = avl.h intavl.h stravl.h

ut_libs = -lgmock -lgmock_main -lgtest -lpthread

.PHONY: all clean ut install deinstall
.SUFFIXES: .c .o .so

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<
.c.so:
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

all: $(targets)
ut: $(ut_targets)
test: $(test_targets)
clean:
	$(RM) $(toclean)
install: all
	for i in $(libdir) $(datadir) $(bindir) $(incdir); \
	do \
		$(INSTALL) $(DMOD) $(OWNER) -d $$i; \
	done
	$(INSTALL) $(FMOD) $(avl_a) $(libdir)/$(avl_a)
	$(INSTALL) $(FMOD) $(avl_fullname) $(libdir)/$(avl_fullname)
	$(LINK) $(avl_fullname) $(libdir)/$(avl_soname)
	$(LINK) $(avl_soname) $(libdir)/$(avl_so)
	for i in $(install_headers);\
	do\
		$(INSTALL) $(FMOD) $(OWNER) $$i $(incdir)/$$i ;\
	done
deinstall:
	$(RM) $(libdir)/$(avl_a)
	$(RM) $(libdir)/$(avl_so)
	$(RM) $(libdir)/$(avl_soname)
	$(RM) $(libdir)/$(avl_fullname)
	for i in $(install_headers);\
	do\
		$(RM) $(incdir)/$$i ;\
	done

$(avl_fullname): $(avl_so_objs) avl.h Makefile
	$(CC) $(CFLAGS) -o $@ -shared -Wl,-soname=$(avl_soname) $(avl_so_objs)
$(avl_soname): $(avl_fullname)
	$(LINK) $(avl_fullname) $(avl_soname)
$(avl_so): $(avl_soname)
	$(LINK) $(avl_soname) $(avl_so)

$(avl_a): $(avl_a_objs)
	$(AR) r $@ $?
	$(RANLIB) $@

tstavl_objs = tstavl.o stravl.o $(avl_a)
tstavl: $(tstavl_objs)
	$(CC) $(LDFLAGS) -o tstavl $(tstavl_objs)

tstavl_so_objs = tstavl.o stravl.o $(avl_so)
tstavl_so: $(tstavl_so_objs)
	$(CC) $(LDFLAGS) -o $@ $(tstavl_so_objs)

tstavl2_objs = tstavl2.o intavl.o $(avl_a)
tstavl2_libs = -lrt
tstavl2: $(tstavl2_objs)
	$(CC) $(LDFLAGS) -o tstavl2 $(tstavl2_objs) $(tstavl2_libs)

tstavl3_objs = tstavl3.o
tstavl3: $(tstavl3_objs)
	$(CC) $(LDFLAGS) -o tstavl3 $(tstavl3_objs)

$(avl_so_objs) $(avl_a_objs): avl.h avlP.h
tstavl.o tstavl2.o tstavl3.o: avl.h
tstavl3.o avl.o: avlP.h

ut_avl_objs = ut_avl.o $(avl_so)

ut_avl: $(ut_avl_objs)
	$(CXX) $(LDFLAGS) -o $@ $(ut_avl_objs) $(ut_libs)

avl.so: avl.h avlP.h

# $Id: Makefile,v 1.5 2014/08/08 19:25:50 luis Exp $

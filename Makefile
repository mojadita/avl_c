# $Id: Makefile,v 1.5 2014/08/08 19:25:50 luis Exp $
# Author: Luis Colorado <lc@luiscoloradosistemas.com>
# Date: jue nov 21 23:48:42 CET 2013
# Disclaimer: (C) 2013 LUIS COLORADO SISTEMAS S.L.U.
# 		All rights reserved.

MAJOR=2
MINOR=1.18
VERSION="$(MAJOR).$(MINOR)"
CFLAGS += -DAVL_VERSION=\"$(VERSION)\"
prefix=/usr

avl_lib_dev		= libavl.so
avl_soname		= $(avl_lib_dev).$(MAJOR)
avl_lib			= $(avl_soname).$(MINOR)
avl_lib_targets = $(avl_lib) $(avl_soname) $(avl_lib_dev)

targets = $(avl_lib_targets) tstavl tstavl2 tstavl3
ut_targets = ut_avl
ut_libs = -lgmock -lgmock_main -lgtest -lpthread

.PHONY: all clean ut

all: $(targets)
ut: $(ut_targets)
clean:
	$(RM) $(targets) $(tstavl_objs)
install: $(targets)
	ln -f $(avl_lib_targets) $(prefix)/lib
	ln -f avl.h $(prefix)/include

$(avl_lib_dev): $(avl_soname) Makefile
	ln -sf $< $@
$(avl_soname): $(avl_lib) Makefile
	ln -sf $< $@
$(avl_lib): avl.c avl.h Makefile
	$(CC) $(CFLAGS) -o $@ -fPIC -shared -Wl,-soname=$(avl_soname) $<

tstavl_objs = tstavl.o stravl.o $(avl_lib)

tstavl: $(tstavl_objs) Makefile
	$(CC) $(LDFLAGS) -o tstavl $(tstavl_objs)

tstavl2_objs = tstavl2.o intavl.o $(avl_lib)
tstavl2: $(tstavl2_objs) Makefile
	$(CC) $(LDFLAGS) -o tstavl2 $(tstavl2_objs)

tstavl3_objs = tstavl3.o
tstavl3: $(tstavl3_objs) Makefile
	$(CC) $(LDFLAGS) -o tstavl3 $(tstavl3_objs)

tstavl3.o avl.o: avlP.h
avl.o tstavl.o tstavl2.o tstavl3.o: avl.h Makefile

ut_avl_objs = ut_avl.o libavl.so

ut_avl: $(ut_avl_objs)
	$(CXX) $(LDFLAGS) -o $@ $(ut_avl_objs) $(ut_libs)

# $Id: Makefile,v 1.5 2014/08/08 19:25:50 luis Exp $

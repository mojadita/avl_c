# $Id: Makefile,v 1.1 2013/11/21 23:01:23 luis Exp $
# Author: Luis Colorado <lc@luiscoloradosistemas.com>
# Date: jue nov 21 23:48:42 CET 2013
# Disclaimer: (C) 2013 LUIS COLORADO SISTEMAS S.L.U.
# 		All rights reserved.

targets = tstavl

.PHONY: all clean

all: $(targets)
clean:
	$(RM) $(targets) $(tstavl_objs)

tstavl_objs = avl.o tstavl.o

tstavl: $(tstavl_objs)
	$(CC) $(LDFLAGS) -o tstavl $(tstavl_objs)

avl.o tstavl.o: avl.h

# $Id: Makefile,v 1.1 2013/11/21 23:01:23 luis Exp $

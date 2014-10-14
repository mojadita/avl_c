#include <stdlib.h>
#include <stdio.h>
#include "avlP.h"

struct avl_malloc {
	struct avl_malloc *next;
};

struct avl_malloc *free_nodes = NULL;

struct avl_malloc {
	struct avl_node[(PAGESIZE)/(

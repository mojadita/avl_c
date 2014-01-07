/* $Id: stravl.h,v 1.1 2014/01/07 20:03:15 luis Exp $
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: Tue Jan  7 20:03:51 CET 2014
 * Disclaimer: (C) 2014 LUIS COLORADO SISTEMAS S.L.U.
 *             All rights reserved.
 */
#ifndef _STRAVL_H
#define _STRAVL_H

#include "avl.h"

typedef int (*STRAVL_FCOMP)(const char *s1, const char *s2);
AVL_TREE new_stravl_tree(STRAVL_FCOMP fc);
#define free_stravl_tree free_avl_tree
#define stravl_tree_clear avl_tree_clear
#define stravl_tree_first avl_tree_first
#define stravl_tree_last avl_tree_last
#define stravl_tree_atkey avl_tree_atkey
AVL_ITERATOR stravl_tree_put(
		AVL_TREE t, /* the tree descriptor */
		const char *k, /* the key string */
		void *d /* the data */
);

int stravl_tree_del(
		AVL_TREE t, /* the tree descriptor */
		const char *s /* the key to be deleted */
);
#define stravl_iterator_del avl_iterator_del
#define stravl_tree_empty avl_tree_empty
#define stravl_tree_size avl_tree_size
int stravl_tree_has(
		AVL_TREE t, /* the tree to be searched */
		const char *k /* the key to be searched for */
);
#define stravl_tree_print avl_tree_print
#define stravl_iterator_next avl_iterator_next
#define stravl_iterator_prev avl_iterator_prev
const char *stravl_iterator_key(
		AVL_ITERATOR i /* iterator to get key from */
);
#define stravl_iterator_data avl_iterator_data

#endif /* _STRAVL_H */
/* $Id: stravl.h,v 1.1 2014/01/07 20:03:15 luis Exp $ */

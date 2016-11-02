/* $Id: intavl.h,v 1.1 2014/01/07 20:03:15 luis Exp $
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: Tue Jan  7 20:03:51 CET 2014
 * Disclaimer: (C) 2014 LUIS COLORADO SISTEMAS S.L.U.
 *             All rights reserved.
 */
#ifndef _INTAVL_H
#define _INTAVL_H

#include "avl.h"

AVL_TREE new_intavl_tree();
#define free_intavl_tree free_avl_tree
#define intavl_tree_clear avl_tree_clear
#define intavl_tree_first avl_tree_first
#define intavl_tree_last avl_tree_last
#define intavl_tree_atkey avl_tree_atkey
AVL_ITERATOR intavl_tree_put(
		AVL_TREE t, /* the tree descriptor */
		const long k, /* the key string */
		void *d /* the data */
);

int intavl_tree_del(
		AVL_TREE t, /* the tree descriptor */
		const long s /* the key to be deleted */
);
#define intavl_iterator_del avl_iterator_del
#define intavl_tree_empty avl_tree_empty
#define intavl_tree_size avl_tree_size
int intavl_tree_has(
		AVL_TREE t, /* the tree to be searched */
		const long k /* the key to be searched for */
);
#define intavl_tree_print avl_tree_print
#define intavl_iterator_next avl_iterator_next
#define intavl_iterator_prev avl_iterator_prev
long intavl_iterator_key(
		AVL_ITERATOR i /* iterator to get key from */
);
#define intavl_iterator_data avl_iterator_data

#endif /* _INTAVL_H */
/* $Id: intavl.h,v 1.1 2014/01/07 20:03:15 luis Exp $ */

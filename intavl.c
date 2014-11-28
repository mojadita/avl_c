/* $Id: intavl.c,v 1.1 2014/01/07 20:03:15 luis Exp $
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: Tue Jan  7 20:21:19 CET 2014
 * Disclaimer: (C) 2014 LUIS COLORADO SISTEMAS S.L.U.
 *             All rights reserved.
 */

#include <stdio.h>
#include "intavl.h"

static int int_print(const void *k, FILE *o)
{
	return fprintf(o, "%d", (int)k);
} /* int_print */

AVL_TREE new_intavl_tree()
{
	return new_avl_tree(NULL, NULL, NULL, int_print);
} /* new intavl_tree */

AVL_ITERATOR intavl_tree_put(
		AVL_TREE t, /* the tree descriptor */
		const int k, /* the key string */
		void *d) /* the data */
{
	return avl_tree_put(t, (void *)k, d);
} /* intavl_tree_put */

int intavl_tree_del(AVL_TREE t, const int k)
{
	return avl_tree_del(t, (void *)k);
} /* intavl_tree_del */

int intavl_tree_has(AVL_TREE t, const int k)
{
	return avl_tree_has(t, (void *)k);
}

int intavl_iterator_key(AVL_ITERATOR i)
{
	return (int) avl_iterator_key(i);
} /* intavl_iterator_key */

/* $Id: intavl.c,v 1.1 2014/01/07 20:03:15 luis Exp $ */

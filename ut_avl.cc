/* avl_ut.cc -- avl unit testing.
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: Thu Nov  6 08:52:34 EET 2014
 * Copyright: (C) 2014 LUIS COLORADO.  All rights reserved.
 */

#include <gtest/gtest.h>

extern "C" {
#include "avl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
} // extern "C"

TEST(Basic, avlTreeCreate) {
	AVL_TREE t;
	EXPECT_NE((AVL_TREE)NULL,
		t = new_avl_tree(
			(AVL_FCOMP)strcmp,
			(AVL_FCONS)strdup,
			(AVL_FDEST)free,
			(AVL_FPRNT)fputs));
}

TEST(Basic, avlTreeFirstNULL) {
	AVL_TREE t;
	EXPECT_NE((AVL_TREE)NULL,
		t = new_avl_tree(
			(AVL_FCOMP)strcmp,
			(AVL_FCONS)strdup,
			(AVL_FDEST)free,
			(AVL_FPRNT)fputs));
	AVL_ITERATOR i;
	EXPECT_EQ((AVL_ITERATOR)NULL,
		i = avl_tree_first(t));
}

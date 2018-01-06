/* $Id: avl.c,v 1.9 2014/08/08 19:10:09 luis Exp $
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: Wed Oct  7 17:57:51     2009
 *
 * Disclaimer:
 *  (C) 2009-2012 LUIS COLORADO SISTEMAS S.L.
 *  All rights reserved.  No part of this code can be distributed in
 *  any form, withour previous written consent of the copyright owner.
 *  This code is distributed WITHOUT ANY WARRANTY, even the implied
 *  WARRANTY OF MERCHANTABILITY or FITNESS FOR ANY PARTICULAR PURPOSE.
 *  In case of an agreed previous, written contract, the WARRANTIES
 *  applied will be the ones written in the applicable contract.
 * Nota: TRANSPORTADO A C en Mon Apr  9 16:48:29 CEST 2012
 * ===== para poder usarlo con flex/bison.
 */

#define IN_AVL_C

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "avlP.h"

#define PAGESIZE	4096U
#define N_NODES	((PAGESIZE)/sizeof(union avl_node_alloc))

#define CRC_INITIAL	(0x2589f3aeUL)

#if USE_CRC
#define ADDCRC(p) do {                 \
        if (!p) break;                 \
		add_crc(CRC_INITIAL,           \
		(CRC_BYTE *)(p), sizeof(*(p)), \
		crc32ieee8023);                \
	} while (0)

#define CRC(p)                     \
	do_crc(CRC_INITIAL,            \
	(CRC_BYTE *)(p), sizeof(*(p)), \
	crc32ieee8023)

#else /* USE_CRC */
#define ADDCRC(p)
#define CRC(p) (0)
#endif

#define BADCRC	" \033[1;33;41mBADCRC\033[m"
/* Standard include files */

/* functions */
static char *avl_equ2str(avl_equ equ)
{
	switch(equ) {
	case AVL_RGT: return "/\\";
	case AVL_EQU: return "()";
	case AVL_LFT: return "\\/";
	default: return "??";
	} /* switch */
} /* avl_equ2str */

static union avl_node_alloc {
	struct avl_node node;
	union avl_node_alloc *next;
} *freeList = NULL;

static struct avl_node *new_avl_node(
	const void *key,
	void *data,
	struct avl_node *prnt,
	AVL_FCONS fc)
{
	struct avl_node *res;

	if (!freeList) {
		int i;
		union avl_node_alloc *p;

		DEB((PR("CALLOC(N_NODES(%lu), "
			"sizeof(union avl_node_alloc)(%lu))"
			" -> MALLOC(PAGESIZE(%u))\n"),
			N_NODES,
			sizeof(union avl_node_alloc),
			PAGESIZE));
		assert(freeList = malloc(PAGESIZE));
		for (i = 0, p = freeList; i < N_NODES-1; i++)
			freeList[i].next = ++p;
		freeList[i].next = NULL;
	} /* if */

	/* unlink from the free list */
	res = &freeList->node;
	freeList = freeList->next;

	/* now initialize */
	res->parent = prnt;
	res->left = NULL;
	res->right = NULL;
	res->equi = AVL_EQU;
	res->key = fc ? fc(key) : key;
	res->data = data;
	ADDCRC(res);
	return res;
} /* new_avl_node */

static void free_avl_node(struct avl_node *n, AVL_FDEST fd)
{
	if (n) {
		union avl_node_alloc *p = (union avl_node_alloc *) n;
		if (n->left) free_avl_node(n->left, fd);
		if (n->right) free_avl_node(n->right, fd);
		DEB(("free_avl_node: deleting %p\n", n));
		if (fd) fd(n->key);

		/* return to the free list */
		p->next = freeList;
		freeList = p;
		/* now calculate CRC */
		ADDCRC(p);
	} /* if */
} /* free_avl_node */

static struct avl_node *avl_node_root(struct avl_node *n)
{
	struct avl_node *res;

	for (res = n; res->parent; res = res->parent)
		continue;
	return res;
} /* avl_node_root */

static struct avl_node *avl_node_first(struct avl_node *n)
{
	struct avl_node *res;

	for (res = n; res->left; res = res->left)
		continue;
	return res;
} /* avl_node_first */

static struct avl_node *avl_node_last(struct avl_node *n)
{
	struct avl_node *res;

	for (res = n; res->right; res = res->right)
		continue;
	return res;
} /* avl_node_last */

static struct avl_node *avl_node_prev(struct avl_node *n)
{
	if (n->left) return avl_node_last(n->left);
	while (n->parent && (n->parent->left == n)) n = n->parent;
	return n->parent;
} /* avl_node_prev */

static struct avl_node *avl_node_next(struct avl_node *n)
{
	if (n->right) return avl_node_first(n->right);
	while (n->parent && (n->parent->right == n)) n = n->parent;
	return n->parent;
} /* avl_node_next */

static int default_fcomp(const void *k1, const void *k2)
{
	return (const char *) k1 - (const char *) k2;
} /* default_fcomp */

static struct avl_node *avl_node_search(
	struct avl_node *n,
	const void *k,
	AVL_FCOMP fc,
	avl_equ *e)
{
	struct avl_node *p = n;

	if (!n) return NULL;
	if (!fc) fc = default_fcomp;

	for (;;) {
		int cmp = fc(k, p->key); /* compare */

		if (cmp > 0) { /* k > p->key */
			if (e) *e = AVL_RGT;
			if (!p->right) break;
			p = p->right;
		} else if (cmp < 0) { /* k < p->key */
			if (e) *e = AVL_LFT;
			if (!p->left) break;
			p = p->left;
		} else { /* k == p->key */
			if (e) *e = AVL_EQU;
			return p;
		} /* if */
	} /* for */
	return p;
} /* avl_node_search */

static struct avl_node *avl_node_unlink(
	struct avl_node * const n,
	struct avl_node ** const rt)
{
	struct avl_node *p = n, *q;
	avl_equ e;
	int disminuido;
	AVL_ITERATOR res;

	/* IF IT HAS BOTH CHILDREN... UNLINK THE NEXT OR THE
	 * PREVIOUS, AND THEN PUT IT HERE (INSTEAD OF THIS
	 * NODE) */
	if (p->left && p->right) {
		switch(p->equi) {
		case AVL_EQU:
		case AVL_LFT: p = avl_node_prev(p); break;
		case AVL_RGT: p = avl_node_next(p); break;
		} /* switch */
	} /* if */

	/* 1.- p POINTS TO THE NODE BEING UNLINKED.
	 * 2.- p HASN'T BOTH CHILDREN. (THIS CASE IS INCLUDED
	 *     IN PREVIOUS CASE), SO WE HAVE TO UNLINK IT AS
	 *     IF IT WERE IN A LINKED LIST. */

	q = p->parent;

	/* UNLINK IT */
	res = p;
	if (q) { /* we have a parent. */
		if (q->left == p) { /* it is left child */
			q->left = p->left ? p->left : p->right;
			if (q->left) {
				q->left->parent = q;
				ADDCRC(q->left);
			} /* if */
			e = AVL_LFT;
		} else { /* q->right == p, it is right child */
			q->right = p->right ? p->right : p->left;
			if (q->right) {
				q->right->parent = q;
				ADDCRC(q->right);
			} /* if */
			e = AVL_RGT;
		} /* if */
		ADDCRC(q);
	} else { /* no parent, we have to touch *rt. */
		*rt = p->left ? p->left : p->right;
		if (*rt) {
			(*rt)->parent = NULL;
			ADDCRC(*rt);
		} /* if */
		p = *rt;
		e = AVL_EQU;
	} /* if */

	/* ADJUST POINTERS IN UNLINKED NODE. */
	res->left = res->right = res->parent = NULL;

	/* res IS READY TO BE RETURNED, AS IT HAS BEEN UNLINKED, NOW
	 * WE HAVE TO GO UP THROUGH THE TREE TO EQUILIBRATE.
	 * WHILE THE TREE HEIGHT HAS DECREASED, WE HAVE TO FIND A NODE
	 * THAT EQUILIBRATES OR ONE THAT DEEQUILIBRATES TOO MUCH. */
	for (disminuido = TRUE; disminuido && q; q = q->parent) {
		switch (e) {
		case AVL_LFT: /* we lose height on the left */
			switch (q->equi) {
			case AVL_LFT: q->equi = AVL_EQU; break;
			case AVL_EQU: q->equi = AVL_RGT; disminuido = FALSE; break;
			case AVL_RGT:
				switch (q->right->equi) {
				case AVL_EQU: disminuido = FALSE;
					/* FALLTHROUGH */
				case AVL_RGT:
					/* in unlinks equilibration always makes the
					 * height to go smaller, so we don't touch 
					 * disminuido variable. */
					avl_node_equilibrateRR(q,
						q->parent
							? q->parent->left == q
								? &q->parent->left
								: &q->parent->right
							: rt);
					break;
				case AVL_LFT:
					/* in unlinks equilibration always makes the
					 * height to go smaller, so we don't touch 
					 * disminuido variable. */
					avl_node_equilibrateRL(q,
						q->parent
							? q->parent->right == q
								? &q->parent->right
								: &q->parent->left
							: rt);
					break;
				} /* switch */
				q = q->parent; /* so it points to the correct node. */
				break;
			} /* switch */
			break;
		case AVL_RGT: /* we lost height on the right. */
			switch(q->equi) {
			case AVL_RGT: q->equi = AVL_EQU; break;
			case AVL_EQU: q->equi = AVL_LFT; disminuido = FALSE; break;
			case AVL_LFT: /* have to equilibrate. */
				switch (q->left->equi) {
				case AVL_EQU: disminuido = FALSE;
					/* FALLTHROUGH */
				case AVL_LFT:
					/* ON UNLINK, THE EQUILIBRATION PROCEDURE ALWAYS
					 * MAKES THE HEIGHT GO DOWN, SO WE DON'T TOUCH
					 * disminuido VARIABLE. */
					avl_node_equilibrateLL(q,
						q->parent
							? q->parent->right == q
								? &q->parent->right
								: &q->parent->left
							: rt);
					break;
				case AVL_RGT:
					/* ON UNLINK, THE EQUILIBRATION PROCEDURE ALWAYS
					 * MAKES THE HEIGHT GO DOWN, SO WE DON'T TOUCH
					 * disminuido VARIABLE. */
					avl_node_equilibrateLR(q,
						q->parent
							? q->parent->left == q
								? &q->parent->left
								: &q->parent->right
							: rt);
					break;
				} /* switch */
				q = q->parent; /* so we point to the correct node. */
				break;
			} /* switch */
			break;
        case AVL_EQU: break;
		} /* switch */
		e = q->parent
			? q->parent->left == q
				? AVL_LFT
				: AVL_RGT
			: AVL_EQU;
		ADDCRC(q);
	} /* for */

	ADDCRC(q);

	/* if unlinked node is not n, that means that n had
	 * both children and we have unlinked the next node
	 * or the previous, so we have to put it in the place
	 * n was occupying, so we can erase n. */
	if (n != res) {
		if (n->parent) {
			if (n->parent->left == n)
				n->parent->left = res;
			else
				n->parent->right = res;
			ADDCRC(n->parent);
		} else { /* we have to touch *rt */
			*rt = res;
		} /* if */
		if (n->left) {
			n->left->parent = res;
			ADDCRC(n->left);
		} /* if */
		if (n->right) {
			n->right->parent = res;
			ADDCRC(n->right);
		} /* if */
		res->parent = n->parent;
		res->left = n->left;
		res->right = n->right;
		res->equi = n->equi;
		ADDCRC(res);
		n->parent = n->left = n->right = NULL;
	} /* if */

	return n;
} /* avl_node_unlink */

static int avl_node_level(struct avl_node *n)
{
	int res = 0;
	while(n) {
		res++;
		n = n->parent;
	} /* while */
	return res;
} /* avl_node_level */

static void avl_node_equilibrateLL(
	struct avl_node *n,
	struct avl_node **const rt)
{
	struct avl_node *l, *r;
	assert(*rt == n);

	/* EQUILIBRATE LL IS CALLED WHEN
	 * INSERTING ON LEFT LEG OF NODE MARKED
	 * +++. (NODE MARKED xxx DOESN'T EXIST)
	 * IT PRODUCES A DIFFERENCE IN NODE n
	 * OF TWO HEIGHTS.
	 * IT ALSO GETS CALLED WHEN UNLINKING
	 * ON RIGHT LEG THE NODE MARKED xxx
	 * (THEN, YOU HAVE ALREADY THE NODE
	 * MARKED +++)
	 *
	 *                   |
	 *                +------+           ---
	 *          +-----| this |-----+
	 *          |     +------+     |
	 *        +---+              +---+   ---
	 *   +----| l |----+         |   |
	 *   |    +---+    |         |   |
	 * +---+         +---+       |   |   ---
	 * |   |         | r |       |   |
	 * |...|         |...|       |...|   ...
	 * |   |         |   |       +---+   ---
	 * |   |         |   |       |xxx|
	 * |   |         |   |       |xxx|
	 * +---+         +---+       +---+   ---
	 * |+++|
	 * |+++|
	 * +---+                             ---
	 */
	DEB(("avl_node_equilibrateLL...\n"));

	l = n->left;
	r = l->right;

	/* first adjust parent pointers. */
	l->parent = n->parent;
	n->parent = l;
	if (r) r->parent = n;

	/* next, descendant pointers. */
	n->left = r;
	l->right = n;

	/* last, the pointer *rt */
	*rt = l;

	/* THIS IS THE SITUATION AFTER EQUILIBRATE
	 * LL, THE TREE GETS EQUILIBRATED ON n AS
	 * ON l (r IS NOT TOUCHED).
	 *
	 *          |
	 *       +------+                    ---
	 *   +---|  l   |--------+
	 *   |   +------+        |
	 * +---+              +-----+        ---
	 * |   |           +--|this |-+
	 * |...|           |  +-----+ |
	 * |   |         +---+      +---+    ---
	 * |   |         | r |      |   |
	 * |   |         |...|      |   |    ...
	 * +---+         |   |      |   |    ---
	 * |+++|         |   |      |   |
	 * |+++|         |   |      |...|
	 * +---+         +---+      +---+    ---
	 *                          |xxx|
	 *                          |xxx|
	 *                          +---+    ---
	 */

	switch(l->equi) {
	case AVL_EQU:
		l->equi = AVL_RGT; n->equi = AVL_LFT;
		break;
	case AVL_LFT:
		l->equi = AVL_EQU; n->equi = AVL_EQU;
		break;
    case AVL_RGT: break;
	} /* switch */
	ADDCRC(n);
	ADDCRC(r);
} /* avl_node_equilibrateLL */

static void avl_node_equilibrateRR(
	struct avl_node *n,
	struct avl_node **const rt)
{
	struct avl_node *l, *r;
	assert(*rt == n);

	/* EQUILIBRATE RR IS CALLED WHEN
	 * INSERTING ON RIGHT LEG OF NODE MARKED
	 * +++. (NODE MARKED xxx DOESN'T EXIST)
	 * IT PRODUCES A DIFFERENCE IN NODE n
	 * OF TWO HEIGHTS.
	 * IT ALSO GETS CALLED WHEN UNLINKING
	 * ON RIGHT LEG THE NODE MARKED xxx
	 * (THEN, YOU HAVE ALREADY THE NODE
	 * MARKED +++)
	 *
	 *              |
	 *          +------+                 ---
	 *    +-----| this |-----+
	 *    |     +------+     |
	 *  +---+              +---+         ---
	 *  |   |         +----| r |----+
	 *  |   |         |    +---+    |
	 *  |   |       +---+         +---+  ---
	 *  |   |       | l |         |   |
	 *  |...|       |...|         |...|  ...
	 *  +---+       |   |         |   |  ---
	 *  |xxx|       |   |         |   |
	 *  |xxx|       |   |         |   |
	 *  +---+       +---+         +---+  ---
	 *                            |+++|
	 *                            |+++|
	 *                            +---+  ---
	 */
	DEB(("avl_node_equilibrateRR...\n"));

	r = n->right;
	l = r->left;

	/* FIRST ADJUST parent POINTERS. */
	r->parent = n->parent;
	n->parent = r;
	if (l) l->parent = n;

	/* THEN DESCENDANT POINTERS. */
	n->right = l;
	r->left = n;

	/* FINALLY, POINTER *rt. */
	*rt = r;

	/* AS CAN BE SEEN, THE TREE GETS
	 * EQUILIBRATED, EITHER IN n, OR IN l
	 * (r DOESN'T GET TOUCHED).
	 *
	 *                       |
	 *                   +------+        ---
	 *          +--------|  r   |---+
	 *          |        +------+   |
	 *        +----+              +---+  ---
	 *     +--|this|--+           |   |
	 *     |  +----+  |           |...|
	 *   +---+      +---+         |   |  ---
	 *   |   |      | l |         |   |
	 *   |   |      |...|         |   |  ...
	 *   |   |      |   |         +---+  ---
	 *   |   |      |   |         |+++|
	 *   |...|      |   |         |+++|
	 *   +---+      +---+         +---+  ---
	 *   |xxx|
	 *   |xxx|
	 *   +---+                           ---
	 */

	switch(r->equi) {
	case AVL_EQU:
		r->equi = AVL_LFT; n->equi = AVL_RGT; break;
	case AVL_RGT:
		r->equi = AVL_EQU; n->equi = AVL_EQU; break;
    case AVL_LFT: break;
	} /* switch */
	ADDCRC(n);
	ADDCRC(l);
} /* avl_node_equilibrateRR */

static void avl_node_equilibrateLR(
	struct avl_node *n,
	struct avl_node **const rt)
{
	struct avl_node *l, *r;
	assert(*rt == n);

	/* SITUATION NOW IS THAT NODE +++ IS
	 * INSERTED (WHEN xxx MARKED NODE DOESN'T
	 * EXIST) OR NODE xxx IS UNLINKED (EXISTING
	 * NODE +++) (ONLY ONE OF TWO +++ MARKED
	 * NODES DO EXIST)
	 * IN THIS CASE, NODE r BECOMES PARENT OF
	 * l AND n, LINKING SUBTREES OF r IN RIGHT
	 * AND LEFT LEGS OF l AND n (RESPECTIVELY).
	 * IN UNLINKING CASE, IT'S POSSIBLE BOTH
	 * LEGS MARKED +++ EXIST, SO r WOULD BE
	 * EQUILIBRATED AND switch MUST CHECK FOR
	 * THAT THREE CASES.
	 *
	 *                       |
	 *                    +------+        ---
	 *          +---------| this |---+
	 *          |         +------+   |
	 *        +----+               +---+  ---
	 *     +--| l  |-----+         |   |
	 *     |  +----+     |         |   |
	 *   +---+         +---+       |   |  ---
	 *   |   |      +--| r |--+    |   |
	 *   |   |      |  +---+  |    |   |
	 *   |   |    +---+     +---+  +---+  ---
	 *   |   |    |   |     |   |  |xxx|        
	 *   |   |    |   |     |   |  |xxx|        
	 *   +---+    +---+     +---+  +---+  ---   
	 *            |+++|     |+++|               
	 *            |+++|     |+++|               
	 *            +---+     +---+         ---   
	 */
	DEB(("avl_node_equilibrateLR...\n"));
	l = n->left;
	r = l->right;

	/* first, adjust the parent pointers: */
	r->parent = n->parent;
	l->parent = r;
	n->parent = r;
	if (r->left) r->left->parent = l;
	if (r->right) r->right->parent = n;

	/* then the descendant pointers: */
	n->left = r->right;
	l->right = r->left;
	r->left = l;
	r->right = n;

	/* finally the equilibriums. */
	switch(r->equi) {
	case AVL_LFT: /* node inserted on the left of r. */
		l->equi = AVL_EQU; n->equi = AVL_RGT;
		break;
	case AVL_RGT: /* node inserted on the right of r. */
		l->equi = AVL_LFT; n->equi = AVL_EQU;
		break;
	case AVL_EQU: /* node unlinked and r equilibrated. */
		l->equi = AVL_EQU; n->equi = AVL_EQU;
		break;
	} /* switch */
	r->equi = AVL_EQU;

	/* lastly, adjust *rt */
	*rt = r;

	/* SCENARIO GETS AS SHOWN:
	 *
	 *                   |
	 *                 +---+              ---
	 *           +-----| r |-----+           
	 *           |     +---+     |           
	 *         +----+         +----+      ---
	 *      +--| l  |-+     +-|this|-+       
	 *      |  +----+ |     | +----+ |       
	 *    +---+     +---+ +---+    +---+  ---
	 *    |   |     |   | |   |    |   |     
	 *    |   |     |   | |   |    |   |     
	 *    |   |     +---+ +---+    |   |  ---
	 *    |   |     |+++| |+++|    |   |     
	 *    |   |     |+++| |+++|    |   |     
	 *    +---+     +---+ +---+    +---+  ---
	 *                             |xxx|     
	 *                             |xxx|     
	 *                             +---+  ---
	 */
	ADDCRC(l);
	ADDCRC(n);
	/* These must be generated as the parent
	 * pointers are modified */
	ADDCRC(l->right);
	ADDCRC(n->left);
} /* avl_node_equilibrateLR */

static void avl_node_equilibrateRL(
	struct avl_node *n,
	struct avl_node **const rt)
{
	struct avl_node *r, *l;
	assert(*rt == n);

	/* SITUATION NOW IS THAT NODE +++ IS
	 * INSERTED (WHEN xxx MARKED NODE DOESN'T
	 * EXIST) OR NODE xxx IS UNLINKED (EXISTING
	 * NODE +++) (ONLY ONE OF TWO +++ MARKED
	 * NODES DO EXIST)
	 * IN THIS CASE, NODE l BECOMES PARENT OF
	 * r AND this, LINKING SUBTREES OF l IN RIGHT
	 * AND LEFT LEGS OF r AND this (RESPECTIVELY).
	 * IN UNLINKING CASE, IT'S POSSIBLE BOTH
	 * LEGS MARKED +++ EXIST, SO r WOULD BE
	 * EQUILIBRATED AND switch MUST CHECK FOR
	 * THAT THREE CASES.
	 *
	 *            |                                    
	 *        +------+                   ---           
	 *    +---| this |---------+                       
	 *    |   +------+         |                       
	 *  +---+               +----+       ---           
	 *  |   |         +-----|  r |--+                  
	 *  |   |         |     +----+  |                  
	 *  |   |       +---+         +---+  ---           
	 *  |   |    +--| l |--+      |   |                
	 *  |   |    |  +---+  |      |   |                
	 *  +---+  +---+     +---+    |   |  ---           
	 *  |xxx|  |   |     |   |    |   |                
	 *  |xxx|  |   |     |   |    |   |                
	 *  +---+  +---+     +---+    +---+  ---           
	 *         |+++|     |+++|                         
	 *         |+++|     |+++|                         
	 *         +---+     +---+           ---           
	 */
	DEB(("avl_node_equilibrateRL...\n"));

	r = n->right;
	l = r->left;

	/* FIRST ADJUST parent POINSTERS: */
	l->parent = n->parent;
	r->parent = l;
	n->parent = l;
	if (l->right) l->right->parent = r;
	if (l->left) l->left->parent = n;

	/* THEN DESCENDANT POINTERS */
	n->right = l->left;
	r->left = l->right;
	l->right = r;
	l->left = n;

	/* NOW, EQUILIBRIUMS: */
	switch (l->equi) {
	case AVL_RGT: /* node inserted on the right of l. */
		r->equi = AVL_EQU; n->equi = AVL_LFT; break;
	case AVL_LFT: /* node inserted on the left of l. */
		r->equi = AVL_RGT; n->equi = AVL_EQU; break;
	case AVL_EQU: /* node erased and l equilibrated. */
		r->equi = AVL_EQU; n->equi = AVL_EQU; break;
	} /* switch */
	l->equi = AVL_EQU;

	/* LAST, ADJUST *rt. */
	*rt = l;

	/* SCENARIO GETS AS SHOWN: 
	 *
	 *                |                      
	 *              +---+                 ---
	 *        +-----| l |-----+              
	 *        |     +---+     |              
	 *      +----+         +----+         ---
	 *    +-|this|-+     +-|  r |--+         
	 *    | +----+ |     | +----+  |         
	 *  +---+    +---+ +---+     +---+    ---
	 *  |   |    |   | |   |     |   |       
	 *  |   |    |   | |   |     |   |       
	 *  |   |    +---+ +---+     |   |    ---
	 *  |   |    |+++| |+++|     |   |       
	 *  |   |    |+++| |+++|     |   |       
	 *  +---+    +---+ +---+     +---+    ---
	 *  |xxx|                                
	 *  |xxx|                                
	 *  +---+                             ---
	 */
	ADDCRC(r);
	ADDCRC(n);
	ADDCRC(n->right);
	ADDCRC(r->left);
} /* avl_node_equilibrateRL */

AVL_TREE new_avl_tree(AVL_FCOMP fc, AVL_FCONS fC, AVL_FDEST fD, AVL_FPRNT fP)
{
	AVL_TREE res;
	assert(res = malloc(sizeof (struct avl_tree)));
	res->sz = 0;
	res->root = NULL;
	res->fcomp = fc;
	res->fcons = fC;
	res->fdest = fD;
	res->fprnt = fP;
	ADDCRC(res);
	return res;
} /* new_avl_tree */

void avl_tree_clear(AVL_TREE t)
{
	free_avl_node(t->root, t->fdest);
	t->root = NULL;
	t->sz = 0;
	ADDCRC(t);
} /* avl_tree_clear */

void free_avl_tree(AVL_TREE t)
{
	avl_tree_clear(t);
	free(t);
} /* free_avl_tree */

AVL_ITERATOR avl_tree_first(AVL_TREE t)
{
	return t->root
		? avl_node_first(t->root)
		: NULL;
} /* avl_tree_first */

AVL_ITERATOR avl_tree_last(AVL_TREE t)
{
	return t->root
		? avl_node_last(t->root)
		: NULL;
} /* avl_tree_last */

AVL_ITERATOR avl_tree_atkey(
	AVL_TREE t,
	const void *k,
	AVL_MT ex)
{
	avl_equ e;
	AVL_ITERATOR n;
	if (!t->root) return NULL;
	n = avl_node_search(t->root, k, t->fcomp, &e);
	switch(ex) {
	case MT_L: switch(e) {
		case AVL_RGT: return n;
		case AVL_EQU:
		case AVL_LFT: return avl_node_prev(n);
		} break;
	case MT_LE: switch(e) {
		case AVL_RGT:
		case AVL_EQU: return n;
		case AVL_LFT: return avl_node_prev(n);
		} break;
	case MT_EQ: switch(e) {
		case AVL_EQU: return n;
        default: break;
		} break;
	case MT_GE: switch(e) {
		case AVL_RGT: return avl_node_next(n);
		case AVL_EQU:
		case AVL_LFT: return n;
		} break;
	case MT_G: switch(e) {
		case AVL_RGT:
		case AVL_EQU: return avl_node_next(n);
		case AVL_LFT: return n;
		} break;
	} /* switch */
	return NULL;
} /* avl_tree_atkey */

AVL_ITERATOR avl_tree_put(
	AVL_TREE t,
	const void *k,
	void *d)
{
	struct avl_node *q;
	avl_equ e;
	int crecido;
	AVL_ITERATOR res;

	if (!t->root) {
		/* FIRST NODE IN THE TREE. */
		t->root = new_avl_node(k, d, NULL, t->fcons);
        ADDCRC(t->root);
		t->sz++;
		ADDCRC(t);
		return t->root;
	} /* if */

	q = avl_node_search(t->root, k, t->fcomp, &e);

	switch(e) {
	case AVL_EQU:
		q->data = d; ADDCRC(q); return q;
	case AVL_LFT:
		res = q->left = new_avl_node(k, d, q, t->fcons);
        ADDCRC(res); break;
	case AVL_RGT:
		res = q->right = new_avl_node(k, d, q, t->fcons);
        ADDCRC(res); break;
	} /* switch */

	t->sz++; 

	for (crecido = TRUE; crecido && q; q = q->parent) {
		switch (e) {
		case AVL_LFT: /* has grown on the left */
			switch (q->equi) {
			case AVL_EQU: q->equi = AVL_LFT; break;
			case AVL_RGT: q->equi = AVL_EQU; crecido = FALSE; break;
			case AVL_LFT:
				switch(q->left->equi) {
				case AVL_LFT: /* left-left grow */
					avl_node_equilibrateLL(q, q->parent
						? (q->parent->left == q)
							? &q->parent->left
							: &q->parent->right
						: &t->root);
					break;
				case AVL_RGT: /* left-right grow */
					avl_node_equilibrateLR(q, q->parent
						? (q->parent->left == q)
							? &q->parent->left
							: &q->parent->right
						: &t->root);
					break;
                case AVL_EQU: break;
				} /* switch */
				q = q->parent; /* so q points to the top node */
				crecido = FALSE; /* tree has not grown in this case. */
				break;
			} /* switch */
			break;
		case AVL_RGT: /* has grown on the right */
			/* p is a right child. Grown on the right. */
			switch (q->equi) {
			case AVL_EQU: q->equi = AVL_RGT; break;
			case AVL_LFT: q->equi = AVL_EQU; crecido = FALSE; break;
			case AVL_RGT:
				switch(q->right->equi) {
				case AVL_RGT: /* right-right grow. */
					avl_node_equilibrateRR(q, q->parent
						? (q->parent->right == q)
							? &q->parent->right
							: &q->parent->left
						: &t->root);
					break;
				case AVL_LFT: /* right-left grow. */
					avl_node_equilibrateRL(q, q->parent
						? (q->parent->right == q)
							? &q->parent->right
							: &q->parent->left
						: &t->root);
					break;
                case AVL_EQU: break;
				} /* switch */
				q = q->parent; /* so q points to the proper node */
				crecido = FALSE; /* tree hasn't grow from here on. */
				break;
			} /* switch */
        case AVL_EQU: break;
		} /* switch */
		e = q->parent
			? q->parent->left == q
				? AVL_LFT
				: AVL_RGT
			: AVL_EQU;
		ADDCRC(q);
	} /* for */

	ADDCRC(q);

	ADDCRC(t);
	return res;
} /* avl_tree_put */

int avl_tree_del(AVL_TREE t, const void *k)
{
	avl_equ e;
	struct avl_node *p;

	if (!t->root) return FALSE;
	p = avl_node_search(t->root, k, t->fcomp, &e);
	if (e != AVL_EQU) return FALSE; /* doesn't exist, we dont erase anything. */

	p = avl_node_unlink(p, &t->root); /* unlink and erase. */
	free_avl_node(p, t->fdest);
	t->sz--;
	ADDCRC(t);

	return TRUE;
} /* avl_tree_del */

int avl_iterator_del(
	AVL_ITERATOR i,
	AVL_TREE t)
{
	struct avl_node *p;
	if (!t->root) return FALSE;
	if (!i) return FALSE;

	p = avl_node_unlink(i, &t->root);
	free_avl_node(p, t->fdest);
	t->sz--;
	ADDCRC(t);
	return TRUE;
} /* avl_iterator_del */

int avl_tree_empty(AVL_TREE t)
{
	return !t->root;
} /* avl_tree_empty */

int avl_tree_size(struct avl_tree *t)
{
	return t->sz;
} /* avl_tree_size */

int avl_tree_has(AVL_TREE t, const void *k)
{
	avl_equ e;

	if (!t->root) return FALSE;
	avl_node_search(t->root, k, t->fcomp, &e);

	return e == AVL_EQU;
} /* avl_tree_has */

void *avl_tree_get(AVL_TREE t, const void *k)
{
	avl_equ e;
	struct avl_node *p;

	if (!t->root) return NULL;
	p = avl_node_search(t->root, k, t->fcomp, &e);

	return (e == AVL_EQU)
		? (void *)p->data  /* eliminate const as it isn't const outside
                              this module */
		: NULL;
} /* avl_tree_get */

AVL_ITERATOR avl_iterator_next(AVL_ITERATOR i)
{
	if (!i) return NULL;
	return avl_node_next(i);
} /* avl_iterator_next */

AVL_ITERATOR avl_iterator_prev(AVL_ITERATOR i)
{
	if (!i) return NULL;
	return avl_node_prev(i);
} /* avl_iterator_prev */

const void *avl_iterator_key(AVL_ITERATOR i)
{
	return i->key;
} /* avl_iterator_key */

void *avl_iterator_data(AVL_ITERATOR i)
{
	return (void *) i->data; /* outside it is not const */
} /* avl_iterator_data */

void avl_iterator_set_data(AVL_ITERATOR i, const void *d)
{
	i->data = d;
} /* avl_iterator_set_data */

/*************************/
/* THE PRINTING ROUTINES */
/*************************/

static char pr_buf[256];
static int pr_n = 0;

static int avl_node_printNode(
	struct avl_node *n,
	FILE *o,
	const char *pfx, AVL_FPRNT fp)
{
	int res = 0;
	strcpy(pr_buf + pr_n, pfx);
	res += fprintf(o,
		PR("%s"
		" k=["),
		pr_buf);
	if (fp) res += fp(n->key, o);
	res += fprintf(o,
		"];"
		" e=%s;"
		" l=%d;"
#if USE_CRC
		" crc=0x%08x%s"
#endif
		, avl_equ2str(n->equi),
		avl_node_level(n)
#if USE_CRC
		, n->crc,
		CRC(n) ? BADCRC : ""
#endif
		);
#if PRINT_ALL
	res += fprintf(o,
		" ptr=%p;"
		" prt=%p;"
		" lft=%p;"
		" rgt=%p;"
		" dat=%p;",
		n,
		n->parent,
		n->left,
		n->right,
		n->data);
#endif
	res += fprintf(o, "\n");
	pr_buf[pr_n] = '\0';
	return res;
} /* avl_node_printNode */
		
static void avl_node_printL(
	struct avl_node *n,
	FILE *o,
	const char *prf,
	AVL_FPRNT pf)
{
	int l = pr_n;
	strcpy(pr_buf + pr_n, prf);
	pr_n += strlen(prf);

	/* WE HAVE USED UTF-8 STRINGS TO REPRESENT
	 * THE TREE AND ITS LEGS. */
	/* RECURSIVE CALL FOR THE RIGHT SIDE */
	if (n->right) avl_node_printR(n->right, o, "\xe2\x94\x82", pf);
	avl_node_printNode(n, o, n->right
		? (n->left
			? "\xe2\x94\x94\xe2\x94\xbc"
			: "\xe2\x94\x94\xe2\x94\xb4")
		: (n->left
			? "\xe2\x94\x94\xe2\x94\xac"
			: "\xe2\x94\x94\xe2\x94\x80"), pf);
	/* RECURSIVE CALL FOR THE LEFT SIDE. */
	if (n->left) avl_node_printL(n->left, o, " ", pf);
	pr_n = l;
	pr_buf[l] = '\0';
} /* avl_node_printL */

static void avl_node_printR(
	struct avl_node *n,
	FILE *o,
	const char *prf, AVL_FPRNT pf)
{
	int l = pr_n;
	strcpy(pr_buf + pr_n, prf);
	pr_n += strlen(prf);

	/* WE HAVE USED UTF-8 STRINGS TO REPRESENT
	 * THE TREE AND ITS LEGS. */
	/* RECURSIVE CALL FOR THE RIGHT SIDE */
	if (n->right) avl_node_printR(n->right, o, " ", pf);
	avl_node_printNode(n, o, n->right
		? (n->left
			? "\xe2\x94\x8c\xe2\x94\xbc"
			: "\xe2\x94\x8c\xe2\x94\xb4")
		: (n->left
			? "\xe2\x94\x8c\xe2\x94\xac"
			: "\xe2\x94\x8c\xe2\x94\x80"), pf);
	/* RECURSIVE CALL FOR THE LEFT SIDE. */
	if (n->left)
		avl_node_printL(n->left, o,
			"\xe2\x94\x82", pf);
	pr_n = l;
	pr_buf[l] = '\0';
} /* avl_node_printR */

static void avl_node_print(struct avl_node *n, FILE *o, AVL_FPRNT pf)
{
	/* WE HAVE USED UTF-8 STRINGS TO REPRESENT
	 * THE TREE AND ITS LEGS. */
	/* CALL FOR THE RIGHT SIDE */
	if (n->right) avl_node_printR(n->right, o, "", pf);
	avl_node_printNode(n, o, n->right
		? (n->left
			? "\xe2\x94\xbc"
			: "\xe2\x94\xb4")
		: (n->left
			? "\xe2\x94\xac"
			: "\xe2\x94\x80"), pf);
	/* RECURSIVE CALL FOR THE LEFT SIDE. */
	if (n->left) avl_node_printL(n->left, o, "", pf);
} /* avl_node_print */

void avl_tree_print(AVL_TREE t, FILE *o)
{
#if DEBUG
	fprintbuf(o,
		sizeof(*t), t,
		PR("TREE: "
#if USE_CRC
		"crc=0x%08x%s"
#endif /* USE_CRC */
		)
#if USE_CRC
		, t->crc, CRC(t) ? BADCRC : ""
#endif /* USE_CRC */
		);
#endif /* DEBUG */
	if (t->root)
		avl_node_print(t->root, o, t->fprnt);
#if USE_CRC
	fprintf(o, PR("TREE: bad crcs = %d/%d\n"),
		avl_tree_chk(t), avl_tree_size(t));
#endif /* USE_CRC */
} /* avl_tree_print */

#if USE_CRC
static int avl_node_chk(
	struct avl_node *n)
{
	int res = 0;
	if (CRC(n)) res++;
	if (n->left) res += avl_node_chk(n->left);
	if (n->right) res += avl_node_chk(n->right);
	return res;
} /* avl_node_chk */
#endif

int avl_tree_chk(
	AVL_TREE t)
{
	int res = 0;
#if USE_CRC
	if (CRC(t)) res++;
	if (t->root) res += avl_node_chk(t->root);
#endif
	return res;
} /* avl_tree_chk */
	
/* $Id: avl.c,v 1.9 2014/08/08 19:10:09 luis Exp $ */
/* vim: ts=4 sw=4 nu ai
 */

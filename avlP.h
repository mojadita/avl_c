/* $Id$
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: Tue Oct 14 14:18:40 EEST 2014
 * Copyright: (C) 2014 LUIS COLORADO SISTEMAS S.L.U.  All rights reserved.
 */

#ifndef _AVLP_H
#define _AVLP_H

#include "avl.h"

/* constants */
#ifndef DEBUG
#define DEBUG 			(0)
#endif

#ifndef USE_CRC
#define USE_CRC			(DEBUG)
#endif

#if USE_CRC
#include <crc.h>
#include <crc_alltables.h>
#endif

#ifdef DEB
#undef DEB
#endif
#if DEBUG
#define DEB(X) printf X
#define PR(X) __FILE__":%d:%s: " X, __LINE__, __func__
#else
#define DEB(X)
#define PR(X) X
#endif

#ifndef PRINT_ALL
#define PRINT_ALL		(DEBUG)
#endif

#ifndef FALSE
#define FALSE 			(0)
#define TRUE			(!FALSE)
#endif

/* types */

typedef enum avl_equ {
	AVL_LFT,
	AVL_EQU,
	AVL_RGT,
} avl_equ;

struct avl_node {
	avl_equ				equi;
	struct avl_node		*parent;
	struct avl_node		*left;
	struct avl_node		*right;
	const void			*key;
	const void			*data;
#if USE_CRC
	CRC_STATE			crc; /* must be the last field */
#endif
};

struct avl_tree {
	int					sz;
	struct avl_node		*root;
	AVL_FCOMP			fcomp;
	AVL_FCONS			fcons;
	AVL_FDEST			fdest;
	AVL_FPRNT			fprnt;
#if USE_CRC
	CRC_STATE			crc; /* must be the last field */
#endif
};

/* private prototypes */
static char *avl_equ2str(avl_equ equ);
static struct avl_node *new_avl_node(
	const void *key,
	void *data,
	struct avl_node *prnt,
	AVL_FCONS fc);
static void free_avl_node(struct avl_node *n, AVL_FDEST fd);
static struct avl_node *avl_node_root(struct avl_node *n);
static struct avl_node *avl_node_first(struct avl_node *n);
static struct avl_node *avl_node_last(struct avl_node *n);
static struct avl_node *avl_node_prev(struct avl_node *n);
static struct avl_node *avl_node_next(struct avl_node *n);
static struct avl_node *avl_node_search(
	struct avl_node *n,
	const void *k,
	AVL_FCOMP fc,
	avl_equ *e);
static struct avl_node *avl_node_unlink(
	struct avl_node *const n,
	struct avl_node **const rt);
static int avl_node_level(struct avl_node *n);
static void avl_node_equilibrateLL(
	struct avl_node *n,
	struct avl_node **const rt);
static void avl_node_equilibrateRR(
	struct avl_node *n,
	struct avl_node **const rt);
static void avl_node_equilibrateLR(
	struct avl_node *n,
	struct avl_node **const rt);
static void avl_node_equilibrateRL(
	struct avl_node *n,
	struct avl_node **const rt);
static int avl_node_printNode(
	struct avl_node *n, FILE *o,
	const char *pfx, AVL_FPRNT fp);
static void avl_node_printL(
	struct avl_node *n, FILE *o,
	const char *pfx, AVL_FPRNT fp);
static void avl_node_printR(
	struct avl_node *n,
	FILE *o,
	const char *pfx, AVL_FPRNT fp);
static void avl_node_print(
	struct avl_node *n,
	FILE *o, AVL_FPRNT fp);
static int avl_node_chk(
	struct avl_node *n);

#endif /* _AVLP_H */
/* avlP.h */

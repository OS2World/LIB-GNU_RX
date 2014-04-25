/* classes: h_files */

#ifndef RXNODEH
#define RXNODEH
/***********************************************************

Copyright 1995 by Tom Lord

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of the copyright holder not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

Tom Lord DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
EVENT SHALL TOM LORD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

******************************************************************/



/*
 * Tom Lord (lord@cygnus.com, lord@gnu.ai.mit.edu)
 */


#include "rxbitset.h"
#include "rxcset.h"



enum rexp_node_type
{
  r_cset,			/* Match from a character set. `a' or `[a-z]'*/
  r_concat,			/* Concat two subexpressions.   `ab' */
  r_alternate,			/* Choose one of two subexpressions. `a\|b' */
  r_opt,			/* Optional subexpression. `a?' */
  r_star,			/* Repeated subexpression. `a*' */
  r_plus,			/* Nontrivially repeated subexpression. `a+' */
  r_interval,			/* Counted subexpression.  `a{4,1000}' */
  r_parens,
  r_context
};


struct rexp_node
{
  int refs;
  enum rexp_node_type type;
  struct
  {
    int cset_size;
    rx_Bitset cset;
    int intval;
    int intval2;
    struct
      {
	struct rexp_node *left;
	struct rexp_node *right;
      } pair;
  } params;
  int id;
  int len;
  int observed;
  struct rexp_node * simplified;
  struct rx_cached_rexp * cr;
};




#ifdef __STDC__
extern struct rexp_node * rexp_node (int type);
extern struct rexp_node * rx_mk_r_cset (int type, int size, rx_Bitset b);
extern struct rexp_node * rx_mk_r_int (int type, int intval);
extern struct rexp_node * rx_mk_r_binop (int type, struct rexp_node * a, struct rexp_node * b);
extern struct rexp_node * rx_mk_r_monop (int type, struct rexp_node * a);
extern void rx_free_rexp (struct rexp_node * node);
extern void rx_save_rexp (struct rexp_node * node);
extern struct rexp_node * rx_copy_rexp (int cset_size, struct rexp_node *node);
extern int rx_rexp_equal (struct rexp_node * a, struct rexp_node * b);
extern unsigned long rx_rexp_hash (struct rexp_node * node, unsigned long seed);

#else /* STDC */
extern struct rexp_node * rexp_node ();
extern struct rexp_node * rx_mk_r_cset ();
extern struct rexp_node * rx_mk_r_int ();
extern struct rexp_node * rx_mk_r_binop ();
extern struct rexp_node * rx_mk_r_monop ();
extern void rx_free_rexp ();
extern void rx_save_rexp ();
extern struct rexp_node * rx_copy_rexp ();
extern int rx_rexp_equal ();
extern unsigned long rx_rexp_hash ();

#endif /* STDC */


#endif  /* RXNODEH */

/* classes: src_files */

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



#include "rxall.h"
#include "rxnode.h"


#ifdef __STDC__
struct rexp_node *
rexp_node (int type)
#else
struct rexp_node *
rexp_node (type)
     int type;
#endif
{
  struct rexp_node *n;

  n = (struct rexp_node *)malloc (sizeof (*n));
  bzero (n, sizeof (*n));
  if (n)
    {
      n->type = type;
      n->id = -1;
      n->refs = 1;
    }
  return n;
}


/* free_rexp_node assumes that the bitset passed to rx_mk_r_cset
 * can be freed using rx_free_cset.
 */

#ifdef __STDC__
struct rexp_node *
rx_mk_r_cset (int type, int size, rx_Bitset b)
#else
struct rexp_node *
rx_mk_r_cset (type, int size, b)
     int type;
     int size;
     rx_Bitset b;
#endif
{
  struct rexp_node * n;
  n = rexp_node (type);
  if (n)
    {
      n->params.cset = b;
      n->params.cset_size = size;
    }
  return n;
}


#ifdef __STDC__
struct rexp_node *
rx_mk_r_int (int type, int intval)
#else
struct rexp_node *
rx_mk_r_int (type, intval)
     int type;
     int intval;
#endif
{
  struct rexp_node * n;
  n = rexp_node (type);
  if (n)
    n->params.intval = intval;
  return n;
}


#ifdef __STDC__
struct rexp_node *
rx_mk_r_binop (int type, struct rexp_node * a, struct rexp_node * b)
#else
struct rexp_node *
rx_mk_r_binop (type, a, b)
     int type;
     struct rexp_node * a;
     struct rexp_node * b;
#endif
{
  struct rexp_node * n = rexp_node (type);
  if (n)
    {
      n->params.pair.left = a;
      n->params.pair.right = b;
    }
  return n;
}


#ifdef __STDC__
struct rexp_node *
rx_mk_r_monop (int type, struct rexp_node * a)
#else
struct rexp_node *
rx_mk_r_monop (type, a)
     int type;
     struct rexp_node * a;
#endif
{
  return rx_mk_r_binop (type, a, 0);
}


#ifdef __STDC__
void
rx_free_rexp (struct rexp_node * node)
#else
void
rx_free_rexp (node)
     struct rexp_node * node;
#endif
{
  if (node && !--node->refs)
    {
      if (node->params.cset)
	rx_free_cset (node->params.cset);
      rx_free_rexp (node->params.pair.left);
      rx_free_rexp (node->params.pair.right);
      rx_free_rexp (node->simplified);
      free ((char *)node);
    }
}

#ifdef __STDC__
void
rx_save_rexp (struct rexp_node * node)
#else
void
rx_save_rexp (node)
     struct rexp_node * node;
#endif
{
  if (node)
    ++node->refs;
}


#ifdef __STDC__
struct rexp_node * 
rx_copy_rexp (int cset_size, struct rexp_node *node)
#else
struct rexp_node * 
rx_copy_rexp (cset_size, node)
     int cset_size;
     struct rexp_node *node;
#endif
{
  if (!node)
    return 0;
  else
    {
      struct rexp_node *n;
      n = rexp_node (node->type);
      if (!n)
	return 0;

      if (node->params.cset)
	{
	  n->params.cset = rx_copy_cset (cset_size,
					 node->params.cset);
	  if (!n->params.cset)
	    {
	      rx_free_rexp (n);
	      return 0;
	    }
	}

      n->params.intval = node->params.intval;
      n->params.pair.left = rx_copy_rexp (cset_size, node->params.pair.left);
      n->params.pair.right = rx_copy_rexp (cset_size, node->params.pair.right);
      if (   (node->params.pair.left && !n->params.pair.left)
	  || (node->params.pair.right && !n->params.pair.right))
	{
	  rx_free_rexp  (n);
	  return 0;
	}
      return n;
    }
}



int re_abs = 0;
int re_ans = 0;

#ifdef __STDC__
int
rx_rexp_equal (struct rexp_node * a, struct rexp_node * b)
#else
int
rx_rexp_equal (a, b)
     struct rexp_node * a;
     struct rexp_node * b;
#endif
{
  int ret;

  if (a == b)
    return 1;

  if ((a == 0) || (b == 0))
    return 0;

  if (a->type != b->type)
    return 0;

  switch (a->type)
    {
    case r_cset:
      ret = (   (a->params.cset_size == b->params.cset_size)
	     && rx_bitset_is_equal (a->params.cset_size,
				    a->params.cset,
				    b->params.cset));
      break;
    case r_concat:
    case r_alternate:
      ret = (   rx_rexp_equal (a->params.pair.left, b->params.pair.left)
	     && rx_rexp_equal (a->params.pair.right, b->params.pair.right));
      break;
    case r_opt:
    case r_star:
    case r_plus:
      ret = rx_rexp_equal (a->params.pair.left, b->params.pair.left);
      break;
    case r_interval:
      ret = (   (a->params.intval == b->params.intval)
	     && (a->params.intval2 == b->params.intval2));
      break;
    case r_parens:
    case r_context:
      ret = (a->params.intval == b->params.intval);
      break;
    default:
      return 0;
    }

  /**/
  ++re_abs;
  re_ans += (ret*2 - 1);

  return ret;
}





#ifdef __STDC__
unsigned long
rx_rexp_hash (struct rexp_node * node, unsigned long seed)
#else
     unsigned long
     rx_rexp_hash (node, seed)
     struct rexp_node * node;
     unsigned long seed;
#endif
{
  if (!node)
    return 0;

  seed += (seed << 3) + rx_rexp_hash (node->params.pair.left, seed);
  seed += (seed << 3) + rx_rexp_hash (node->params.pair.right, seed);
  seed += (seed << 3) + rx_bitset_hash (node->params.cset_size, node->params.cset);
  seed += (seed << 3) + node->params.intval;
  seed += (seed << 3) + node->params.intval2;
  seed += (seed << 3) + node->type;
  seed += (seed << 3) + node->id;
  seed += (seed << 3) + node->len;
  seed += (seed << 3) + node->observed;
  return seed;
}

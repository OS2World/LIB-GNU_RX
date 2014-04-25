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
#include "rxhash.h"

static int
rexp_node_equal (va, vb)
     void * va;
     void * vb;
{
  struct rexp_node * a;
  struct rexp_node * b;

  a = (struct rexp_node *)va;
  b = (struct rexp_node *)vb;

  return (   (va == vb)
	  || (   (a->type == b->type)
	      && (a->params.intval == b->params.intval)
	      && (a->params.intval2 == b->params.intval2)
	      && rx_bitset_is_equal (a->params.cset_size, a->params.cset, b->params.cset)
	      && rexp_node_equal (a->params.pair.left, b->params.pair.left)
	      && rexp_node_equal (a->params.pair.right, b->params.pair.right)));
}


static struct rx_hash_rules rexp_node_hash_rules =
{
  rexp_node_equal,
  0,
  0,
  0,
  0
};



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
#include "rxsimp.h"




/* Could reasonably hashcons instead of in rxunfa.c */

#ifdef __STDC__
int
rx_simple_rexp (struct rexp_node ** answer,
		int cset_size,
		struct rexp_node *node,
		struct rexp_node ** subexps)
#else
int
rx_simple_rexp (answer, cset_size, node, subexps)
     struct rexp_node ** answer;
     int cset_size;
     struct rexp_node *node;
     struct rexp_node ** subexps;
#endif
{
  int stat;

  if (!node)
    {
      *answer = 0;
      return 0;
    }

  if (!node->observed)
    {
      rx_save_rexp (node);
      *answer = node;
      return 0;
    }

  if (node->simplified)
    {
      rx_save_rexp (node->simplified);
      *answer = node->simplified;
      return 0;
    }

  switch (node->type)
    {
    default:
    case r_cset:
      return -2;		/* an internal error, really */

    case r_parens:
      stat = rx_simple_rexp (answer, cset_size,
			     node->params.pair.left,
			     subexps);
      break;

    case r_context:
      if (isdigit (node->params.intval))
	 stat = rx_simple_rexp (answer, cset_size,
				subexps [node->params.intval - '0'],
				subexps);
      else
	{
	  *answer = 0;
	  stat = 0;
	}
      break;

    case r_concat:
    case r_alternate:
    case r_opt:
    case r_star:
    case r_plus:
    case r_interval:
      {
	struct rexp_node *n;
	n = rexp_node (node->type);
	if (!n)
	  return -1;

	if (node->params.cset)
	  {
	    n->params.cset = rx_copy_cset (cset_size,
					   node->params.cset);
	    if (!n->params.cset)
	      {
		rx_free_rexp (n);
		return -1;
	      }
	  }
	n->params.intval = node->params.intval;
	n->params.intval2 = node->params.intval2;
	{
	  int s;
    
	  if (!(s = rx_simple_rexp (&n->params.pair.left, cset_size,
				    node->params.pair.left, subexps))
	      && !( s= rx_simple_rexp (&n->params.pair.right, cset_size,
				       node->params.pair.right, subexps)))
	    {
	      *answer = n;
	      stat = 0;
	    }
	  else
	    {
	      rx_free_rexp  (n);
	      stat = s;
	    }
	}
      }      
      break;
    }

  if (!stat)
    {
      node->simplified = *answer;
      rx_save_rexp (node->simplified);
    }
  return stat;
}  



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
#include "rxgnucomp.h"
#include "rxnfa.h"

#include <stdio.h>


#ifdef __GNUC__
#define AT(X) [X] =
#else
#define AT(X)
#endif


char *node_type_names[] =
{
  AT(r_cset) "r_cset",
  AT(r_concat) "r_concat",
  AT(r_alternate) "r_alternate",
  AT(r_opt) "r_opt",
  AT(r_star) "r_star",
  AT(r_plus) "r_plus",
  AT(r_interval) "r_interval",
  AT(r_parens) "r_parens",
  AT(r_context) "r_conext"
};

void
print_cset (cset_size, cs)
     int cset_size;
     rx_Bitset cs;
{
  int x;
  if (!cs)
      printf ("nil");
  else
    {
      putchar ('[');
      for (x = 0; x < cset_size; ++x)
	if (RX_bitset_member (cs, x))
	  {
	    if (isprint(x))
	      putchar (x);
	    else
	      printf ("\\0%o ", x);
	  }
      putchar (']');
    }
}

void
spaces (n)
     int n;
{
  while (n--)
    putchar (' ');
}

void
print_rexp (cset_size, indent, rexp)
     int cset_size;
     int indent;
     struct rexp_node * rexp;
{
  spaces (indent);
  if (!rexp)
    printf ("nil\n");
  else
    {
      printf ("Node %d type %d (%s), iv=%d(%c), iv2=%d, len=%d obs=%d cs=",
	      rexp->id, rexp->type, node_type_names[rexp->type],
	      rexp->params.intval,
	      (isprint (rexp->params.intval)
	       ? rexp->params.intval
	       : ' '),
	      rexp->params.intval2,
	      rexp->len,
	      rexp->observed);
      print_cset (cset_size, rexp->params.cset);
      putchar ('\n');
      if (rexp->params.pair.left || rexp->params.pair.right)
	{
	  print_rexp (cset_size, indent + 2, rexp->params.pair.left);
	  print_rexp (cset_size, indent + 2, rexp->params.pair.right);
	}
    }
}




void
unparse_print_rexp (cset_size, rexp)
     int cset_size;
     struct rexp_node * rexp;
{
  if (!rexp)
    return;
  else
    switch (rexp->type)
      {
      case r_cset:
	if (1 != rx_bitset_population (cset_size, rexp->params.cset))
	  print_cset (cset_size, rexp->params.cset);
	else
	  {
	    int x;
	    rx_Bitset cs;
	    
	    cs = rexp->params.cset;
	    for (x = 0; x < cset_size; ++x)
	      if (RX_bitset_member (cs, x))
		{
		  if (isprint(x))
		    putchar (x);
		  else
		    printf ("\\0%o ", x);
		}
	  }
	break;

      case r_parens:
	putchar ('(');
	unparse_print_rexp (cset_size, rexp->params.pair.left);
	putchar (')');
	break;

      case r_context:
	putchar ('\\');
	putchar (rexp->params.intval);
	break;

      case r_concat:
	unparse_print_rexp (cset_size, rexp->params.pair.left);
	unparse_print_rexp (cset_size, rexp->params.pair.right);
	break;

      case r_alternate:
	unparse_print_rexp (cset_size, rexp->params.pair.left);
	putchar ('|');
	unparse_print_rexp (cset_size, rexp->params.pair.right);
	break;

      case r_opt:
	unparse_print_rexp (cset_size, rexp->params.pair.left);
	putchar ('?');
	break;

      case r_star:
	unparse_print_rexp (cset_size, rexp->params.pair.left);
	putchar ('*');
	break;

      case r_plus:
	unparse_print_rexp (cset_size, rexp->params.pair.left);
	putchar ('+');
	break;

      case r_interval:
	unparse_print_rexp (cset_size, rexp->params.pair.left);
	printf ("{%d,%d}", rexp->params.intval, rexp->params.intval2);
	break;
      }
}


void
print_nfa_state (rx, state)
     struct rx * rx;
     struct rx_nfa_state * state;
{
  struct rx_nfa_edge * e;
  printf ("state %d, is_final %d, is_start %d\n",
	  state->id, state->is_final, state->is_start);
  for (e = state->edges; e; e = e->next)
    {
      printf ("\tEdge %s to %d ",
	      (e->type == ne_cset
	       ? "cset"
	       : (e->type == ne_epsilon
		  ? "epsilon"
		  : "side effect")),
	      e->dest->id);
      if (e->type == ne_cset)
	print_cset (rx->local_cset_size, e->params.cset);
      else
	printf ("%d", (int)e->params.side_effect);
      putchar ('\n');
    }
}

void
print_nfa (rx)
     struct rx * rx;
{
  struct rx_nfa_state * state;
  for (state = rx->nfa_states; state; state = state->next)
    print_nfa_state (rx, state);
}

#ifndef RXUNFAH
#define RXUNFAH

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



#include "_rx.h"


struct rx_unfaniverse 
{
  int delay;
  int delayed;
  struct rx_hash table;
  struct rx_cached_rexp * free_queue;
};


struct rx_unfa
{
  int refs;
  struct rexp_node * exp;
  struct rx * nfa;
  int cset_size;
  struct rx_unfaniverse * verse;
};

struct rx_cached_rexp
{
  struct rx_unfa unfa;
  struct rx_cached_rexp * next;
  struct rx_cached_rexp * prev;
  struct rx_hash_item * hash_item;
};



#ifdef __STDC__
extern struct rx * rx_unfa_rx (struct rx_cached_rexp * cr, struct rexp_node * exp, int cset_size);
extern struct rx_unfaniverse * rx_make_unfaniverse (int delay);
extern void rx_free_unfaniverse (struct rx_unfaniverse * it);
extern struct rx_unfa * rx_unfa (struct rx_unfaniverse * unfaniverse, struct rexp_node * exp, int cset_size);
extern void rx_free_unfa (struct rx_unfa * unfa);

#else /* STDC */
extern struct rx * rx_unfa_rx ();
extern struct rx_unfaniverse * rx_make_unfaniverse ();
extern void rx_free_unfaniverse ();
extern struct rx_unfa * rx_unfa ();
extern void rx_free_unfa ();

#endif /* STDC */








#endif  /* RXUNFAH */

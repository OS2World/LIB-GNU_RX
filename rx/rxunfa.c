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
#include "rx.h"
#include "rxunfa.h"
#include "rxnfa.h"


#ifdef __STDC__
static int
unfa_equal (void * va, void * vb)
#else
static int
unfa_equal (va, vb)
     void * va;
     void * vb;
#endif
{
  return rx_rexp_equal ((struct rexp_node *)va, (struct rexp_node *)vb);
}


struct rx_hash_rules unfa_rules = { unfa_equal, 0, 0, 0, 0 };


#ifdef __STDC__
static struct rx_cached_rexp *
canonical_unfa (struct rx_hash * table, struct rexp_node * rexp, int cset_size)
#else
static struct rx_cached_rexp *
canonical_unfa (table, rexp, cset_size)
     struct rx_hash * table;
     struct rexp_node * rexp;
     int cset_size;
#endif
{
  struct rx_hash_item * it;
  it = rx_hash_store (table, rx_rexp_hash (rexp, 0), rexp, &unfa_rules);

  if (it->binding == 0)
    {
      static int rx_id = 0;
      struct rx * new_rx;
      struct rx_cached_rexp * cr;

      if (it->data == (void *)rexp)
	rx_save_rexp (rexp);
      
      cr = (struct rx_cached_rexp *)malloc (sizeof (*cr));
      bzero (cr, sizeof (*cr));
      if (!cr)
	return 0;
      it->binding = (void *)cr;
      cr->unfa.nfa = 0;
      cr->unfa.exp = rexp;
      cr->hash_item = it;
      rx_save_rexp (rexp);
    }
  return (struct rx_cached_rexp *)it->binding;
}


#ifdef __STDC__
struct rx *
rx_unfa_rx (struct rx_cached_rexp * cr, struct rexp_node * exp, int cset_size)
#else
struct rx *
rx_unfa_rx (cr, exp, cset_size)
     struct rx_cached_rexp * cr;
     struct rexp_node * exp;
     int cset_size;
#endif
{
  struct rx * new_rx;

  if (cr->unfa.nfa)
    return cr->unfa.nfa;

  new_rx = rx_make_rx (cset_size);
  if (!new_rx)
    return 0;
  {
    struct rx_nfa_state * start;
    struct rx_nfa_state * end;
    start = end = 0;
    if (!rx_build_nfa (new_rx, exp, &start, &end))
      {
	free (new_rx);
	return 0;
      }
    new_rx->start_nfa_states = start;
    end->is_final = 1;
    start->is_start = 1;
    {
      struct rx_nfa_state * s;
      int x;
      x = 0;
      for (s = new_rx->nfa_states; s; s = s->next)
	s->id = x++;
    }
  }
  cr->unfa.nfa = new_rx;
  return new_rx;
}




#ifdef __STDC__
struct rx_unfaniverse *
rx_make_unfaniverse (int delay)
#else
struct rx_unfaniverse *
rx_make_unfaniverse (delay)
     int delay;
#endif
{
  struct rx_unfaniverse * it;
  it = (struct rx_unfaniverse *)malloc (sizeof (*it));
  if (!it) return 0;
  bzero (it, sizeof (*it));
  it->delay = delay;
  return it;
}


#ifdef __STDC__
void
rx_free_unfaniverse (struct rx_unfaniverse * it)
#else
void
rx_free_unfaniverse (it)
     struct rx_unfaniverse * it;
#endif
{
}





#ifdef __STDC__
struct rx_unfa *
rx_unfa (struct rx_unfaniverse * unfaniverse, struct rexp_node * exp, int cset_size)
#else
struct rx_unfa *
rx_unfa (unfaniverse, exp, cset_size)
     struct rx_unfaniverse * unfaniverse;
     struct rexp_node * exp;
     int cset_size;
#endif
{
  struct rx_cached_rexp * cr;
  if (exp && exp->cr)
    cr = exp->cr;
  else
    {
      cr = canonical_unfa (&unfaniverse->table, exp, cset_size);
      if (exp)
	exp->cr = cr;
    }
  if (!cr)
    return 0;
  if (cr->next)
    {
      if (unfaniverse->free_queue == cr)
	{
	  unfaniverse->free_queue = cr->next;
	  if (unfaniverse->free_queue == cr)
	    unfaniverse->free_queue = 0;
	}
      cr->next->prev = cr->prev;
      cr->prev->next = cr->next;
      cr->next = 0;
      cr->prev = 0;
      --unfaniverse->delayed;
    }
  ++cr->unfa.refs;
  cr->unfa.cset_size = cset_size;
  cr->unfa.verse = unfaniverse;
  rx_unfa_rx (cr, exp, cset_size);
  return &cr->unfa;
}


#ifdef __STDC__
void
rx_free_unfa (struct rx_unfa * unfa)
#else
void
rx_free_unfa (unfa)
     struct rx_unfa * unfa;
#endif
{
  struct rx_cached_rexp * cr;

  cr = (struct rx_cached_rexp *)unfa;
  if (!cr)
    return;
  if (!--cr->unfa.refs)
    {
      if (!unfa->verse->free_queue)
	{
	  unfa->verse->free_queue = cr;
	  cr->next = cr->prev = cr;
	}
      else
	{
	  cr->next = unfa->verse->free_queue;
	  cr->prev = unfa->verse->free_queue->prev;
	  cr->next->prev = cr;
	  cr->prev->next = cr;
	}

      ++unfa->verse->delayed;
      while (unfa->verse->delayed > unfa->verse->delay)
	{
	  struct rx_cached_rexp * it;
	  it = unfa->verse->free_queue;
	  unfa->verse->free_queue = it->next;
	  if (!--unfa->verse->delayed)
	    unfa->verse->free_queue = 0;
	  it->prev->next = it->next;
	  it->next->prev = it->prev;
	  if (it->unfa.exp)
	    it->unfa.exp->cr = 0;
	  rx_free_rexp ((struct rexp_node *)it->hash_item->data);
	  rx_hash_free (it->hash_item, &unfa_rules);
	  rx_free_rx (it->unfa.nfa);
	  rx_free_rexp (it->unfa.exp);
	  free (it);
	  if (it == cr)
	    break;
	}
    }
  else
    return;
}
















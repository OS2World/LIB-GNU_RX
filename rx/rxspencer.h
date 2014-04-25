/* classes: h_files */

#ifndef RXSPENCERH
#define RXSPENCERH
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



#include "rxproto.h"
#include "rxnode.h"
#include "rxunfa.h"
#include "rxanal.h"



typedef struct rx_registers
{
  regoff_t rm_so;  /* Byte offset from string's start to substring's start.  */
  regoff_t rm_eo;  /* Byte offset from string's start to substring's end.  */
} regmatch_t;


typedef enum rx_answers (*rx_vmfn)
     P((void * closure,
	unsigned char ** burst, int * len, int * offset,
	int start, int end, int need));

typedef enum rx_answers (*rx_contextfn)
     P((void * closure,
	int type,
	int start, int end,
	struct rx_registers * regs));


struct rx_solutions
{
  int step;

  int cset_size;
  struct rexp_node * exp;
  struct rexp_node ** subexps;
  struct rx_registers * regs;

  int start;
  int end;

  rx_vmfn vmfn;
  rx_contextfn contextfn;
  void * closure;
  int current_pos;

  struct rx_unfaniverse * verse;
  struct rx_unfa * dfa;
  struct rx_classical_system match_engine;

  int split_guess;
  struct rx_solutions * left;
  struct rx_solutions * right;

  int interval_x;
};


#ifdef __STDC__
extern struct rx_solutions * rx_make_solutions (struct rx_registers * regs,
						struct rx_unfaniverse * verse,
						struct rexp_node * expression,
						struct rexp_node ** subexps,
						int cset_size,
						int start, int end,
						rx_vmfn vmfn, rx_contextfn contextfn,
						void * closure);
extern void rx_free_solutions (struct rx_solutions * solns);
extern enum rx_answers rx_next_solution (struct rx_solutions * solns);

#else /* STDC */
extern struct rx_solutions * rx_make_solutions ();
extern void rx_free_solutions ();
extern enum rx_answers rx_next_solution ();

#endif /* STDC */










#endif  /* RXSPENCERH */

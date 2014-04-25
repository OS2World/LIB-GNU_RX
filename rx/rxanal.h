/* classes: h_files */

#ifndef RXANALH
#define RXANALH
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



#include "rxcset.h"
#include "rxnode.h"
#include "rxsuper.h"




enum rx_answers
{
  rx_yes = 0,
  rx_no = 1,
  rx_maybe = 2,
  rx_bogus = -1,
  rx_start_state_with_too_many_futures = rx_bogus - 1,
  /* n < 0 -- error */
};

struct rx_classical_system
{
  struct rx * rx;
  struct rx_superstate * state;
};



#ifdef __STDC__
extern int rx_posix_analyze_rexp (struct rexp_node *** subexps,
				  int * n_subexps,
				  struct rexp_node * node,
				  int id);
extern int rx_fill_in_fastmap (int cset_size, unsigned char * map, struct rexp_node * exp);
extern int rx_is_anchored_p (struct rexp_node * exp);
extern enum rx_answers rx_start_superstate (struct rx_classical_system * frame);
extern enum rx_answers rx_match_here_p (struct rx_classical_system * frame,
					unsigned char * burst,
					int len);
extern enum rx_answers rx_fit_p (struct rx_classical_system * frame,
				 unsigned char * burst,
				 int len);
extern enum rx_answers rx_longest (int * last_matched,
				   int base_addr,
				   struct rx_classical_system * frame,
				   unsigned char * burst,
				   int len);
extern enum rx_answers rx_advance (struct rx_classical_system * frame,
				   unsigned char * burst, int len);
extern void rx_terminate_system (struct rx_classical_system * frame);
extern void rx_init_system (struct rx_classical_system * frame, struct rx * rx);

#else /* STDC */
extern int rx_posix_analyze_rexp ();
extern int rx_fill_in_fastmap ();
extern int rx_is_anchored_p ();
extern enum rx_answers rx_start_superstate ();
extern enum rx_answers rx_match_here_p ();
extern enum rx_answers rx_fit_p ();
extern enum rx_answers rx_longest ();
extern enum rx_answers rx_advance ();
extern void rx_terminate_system ();
extern void rx_init_system ();

#endif /* STDC */












#endif  /* RXANALH */

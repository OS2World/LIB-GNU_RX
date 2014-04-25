/* classes: h_files */

#ifndef RXBASICH
#define RXBASICH
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



#include "rxcontext.h"
#include "rxnode.h"
#include "rxspencer.h"


#ifdef __STDC__
extern struct rx_solutions * rx_basic_make_solutions (struct rx_registers * regs,
						      struct rexp_node * expression,
						      struct rexp_node ** subexps,
						      int start, int end,
						      struct rx_context_rules * rules,
						      char * str);
extern void rx_basic_free_solutions (struct rx_solutions * solns);

#else /* STDC */
extern struct rx_solutions * rx_basic_make_solutions ();
extern void rx_basic_free_solutions ();

#endif /* STDC */





#endif  /* RXBASICH */

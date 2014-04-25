/* classes: h_files */

#ifndef RXSTRH
#define RXSTRH
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




#include "rxspencer.h"
#include "rxcontext.h"



  

struct rx_str_closure
{
  struct rx_context_rules rules;
  unsigned char * str;
  int len;
};



#ifdef __STDC__
extern enum rx_answers rx_str_vmfn (void * closure,
				    unsigned char ** burstp,
				    int * lenp,
				    int * offsetp,
				    int start, int end, int need);
extern enum rx_answers rx_str_contextfn (void * closure,
					 int type, int start, int end,
					 struct rx_registers * regs);

#else /* STDC */
extern enum rx_answers rx_str_vmfn ();
extern enum rx_answers rx_str_contextfn ();

#endif /* STDC */



#endif  /* RXSTRH */

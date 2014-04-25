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
#include "rxbasic.h"
#include "rxunfa.h"
#include "rxstr.h"




int rx_basic_unfaniverse_delay = 64;
struct rx_unfaniverse * rx_basic_uv = 0;



static int
init_basic_once ()
{
  if (rx_basic_uv)
    return 0;
  rx_basic_uv = rx_make_unfaniverse (rx_basic_unfaniverse_delay);
  return (rx_basic_uv ? 0 : -1);
}


static char * silly_hack = 0;

#ifdef __STDC__
struct rx_solutions *
rx_basic_make_solutions (struct rx_registers * regs,
			 struct rexp_node * expression,
			 struct rexp_node ** subexps,
			 int start, int end,
			 struct rx_context_rules * rules,
			 char * str)
#else
struct rx_solutions *
rx_basic_make_solutions (regs, rexp, subexps, start, end, rules, str)
     struct rx_registers * regs;
     struct rexp_node * expression;
     struct rexp_node ** subexps;
     int start;
     int end;
     struct rx_context_rules * rules;
     char * str;
#endif
{
  struct rx_str_closure * closure;
  if (init_basic_once ())
    return 0;
  if (silly_hack)
    {
      closure = (struct rx_str_closure *)silly_hack;
      silly_hack = 0;
    }
  else
    closure = (struct rx_str_closure *)malloc (sizeof (*closure));
  if (!closure)
    return 0;
  closure->str = str;
  closure->len = end;
  closure->rules = *rules;
  return rx_make_solutions (regs, rx_basic_uv, expression, subexps, 256,
			    start, end, rx_str_vmfn, rx_str_contextfn,
			    (void *)closure);
}



#ifdef __STDC__
void
rx_basic_free_solutions (struct rx_solutions * solns)
#else
     void
     rx_basic_free_solutions (solns)
     struct rx_solutions * solns;
#endif
{
  if (!silly_hack)
    silly_hack = (char *)solns->closure;
  else
    free (solns->closure);
  solns->closure = 0;
  rx_free_solutions (solns);
}

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
#include "rxstr.h"



#ifdef __STDC__
enum rx_answers
rx_str_vmfn (void * closure,
	     unsigned char ** burstp,
	     int * lenp,
	     int * offsetp,
	     int start, int end, int need)
#else
enum rx_answers
rx_str_vmfn (closure, burstp, lenp, offsetp, start, end, need)
     void * closure;
     unsigned char ** burstp;
     int * lenp;
     int * offsetp;
     int start;
     int end;
     int need;
#endif
{
  struct rx_str_closure * strc;
  strc = (struct rx_str_closure *)closure;

  if (   (need < 0)
      || (need > strc->len))
    return rx_no;

  *burstp = strc->str;
  *lenp = strc->len;
  *offsetp = 0;
  return rx_yes;
}

#ifdef __STDC__
enum rx_answers
rx_str_contextfn (void * closure,
		  int type, int start, int end,
		  struct rx_registers * regs)
#else
enum rx_answers
rx_str_contextfn (closure, type, start, end, regs)
     void * closure;
     int type;
     int start;
     int end;
     struct rx_registers * regs;
#endif
{
  struct rx_str_closure * strc;

  strc = (struct rx_str_closure *)closure;
  switch (type)
    {
    case '1': case '2': case '3': case '4': case '5':
    case '6': case '7': case '8': case '9':
      {
	int cmp;
	int regn;
	regn = type - '0';
	if (   (regs[regn].rm_so == -1)
	    || ((end - start) != (regs[regn].rm_eo - regs[regn].rm_so)))
	  return rx_no;
	else
	  {
	    cmp = strncmp (strc->str + start,
			   strc->str + regs[regn].rm_so,
			   end - start);
	    return (!cmp
		    ? rx_yes
		    : rx_no);
	  }
      }

    case '^':
      {
	return ((   (start == end)
		 && (   ((start == 0) && !strc->rules.not_bol)
		     || (   (start > 0)
			 && strc->rules.newline_anchor
			 && (strc->str[start - 1] == '\n'))))
		? rx_yes
		: rx_no);
      }

    case '$':
      {
	return ((   (start == end)
		 && (   ((start == strc->len) && !strc->rules.not_eol)
		     || (   (start < strc->len)
			 && strc->rules.newline_anchor
			 && (strc->str[start] == '\n'))))
		? rx_yes
		: rx_no);
      }

    case '<':
    case '>':

    case 'B':
    case 'b':


    default:
      return rx_bogus;
    }
}

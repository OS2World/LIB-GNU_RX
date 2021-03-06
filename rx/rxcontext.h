/* classes: h_files */

#ifndef RXCONTEXTH
#define RXCONTEXTH
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



struct rx_context_rules
{
  unsigned int newline_anchor:1;/* If true, an anchor at a newline matches.*/
  unsigned int not_bol:1;	/* If set, the anchors ('^' and '$') don't */
  unsigned int not_eol:1;	/*     match at the ends of the string.  */  
};


#ifdef __STDC__

#else /* STDC */

#endif /* STDC */


#endif  /* RXCONTEXTH */

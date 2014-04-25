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


/*
 * Tom Lord (lord@cygnus.com, lord@gnu.ai.mit.edu)
 */



#include "rxall.h"
#include "rxcset.h"
/* Utilities for manipulating bitset represntations of characters sets. */

#ifdef __STDC__
rx_Bitset
rx_cset (int size)
#else
rx_Bitset
rx_cset (size)
     int size;
#endif
{
  rx_Bitset b;
  b = (rx_Bitset) malloc (rx_sizeof_bitset (size));
  if (b)
    rx_bitset_null (size, b);
  return b;
}


#ifdef __STDC__
rx_Bitset
rx_copy_cset (int size, rx_Bitset a)
#else
rx_Bitset
rx_copy_cset (size, a)
     int size;
     rx_Bitset a;
#endif
{
  rx_Bitset cs;
  cs = rx_cset (size);

  if (cs)
    rx_bitset_union (size, cs, a);

  return cs;
}


#ifdef __STDC__
void
rx_free_cset (rx_Bitset c)
#else
void
rx_free_cset (c)
     rx_Bitset c;
#endif
{
  if (c)
    free ((char *)c);
}


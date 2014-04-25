/* classes: h_files */

#ifndef RXCSETH
#define RXCSETH

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

/*  lord	Sun May  7 12:34:11 1995	*/


#include "rxbitset.h"


#ifdef __STDC__
extern rx_Bitset rx_cset (int size);
extern rx_Bitset rx_copy_cset (int size, rx_Bitset a);
extern void rx_free_cset (rx_Bitset c);

#else /* STDC */
extern rx_Bitset rx_cset ();
extern rx_Bitset rx_copy_cset ();
extern void rx_free_cset ();

#endif /* STDC */



#endif  /* RXCSETH */

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
#include "rxhash.h"
#include "rxnfa.h"



const char rx_version_string[] = "GNU Rx version 1.0";


#ifdef __STDC__
struct rx *
rx_make_rx (int cset_size)
#else
struct rx *
rx_make_rx (cset_size)
     int cset_size;
#endif
{
  static int rx_id = 0;
  struct rx * new_rx;
  new_rx = (struct rx *)malloc (sizeof (*new_rx));
  bzero (new_rx, sizeof (*new_rx));
  new_rx->rx_id = rx_id++;
  new_rx->cache = rx_default_cache;
  new_rx->local_cset_size = cset_size;
  new_rx->instruction_table = rx_id_instruction_table;
  new_rx->next_nfa_id = 0;
  return new_rx;
}

#ifdef __STDC__
void
rx_free_rx (struct rx * rx)
#else
void
rx_free_rx (rx)
     struct rx * rx;
#endif
{
  rx_free_nfa (rx);
  free (rx);
}

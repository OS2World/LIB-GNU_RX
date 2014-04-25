/* classes: h_files */

#ifndef RXBITSETH
#define RXBITSETH

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




typedef unsigned long RX_subset;
#define RX_subset_bits	(8 * sizeof (RX_subset))
#define RX_subset_mask	(RX_subset_bits - 1)
typedef RX_subset * rx_Bitset;

#ifdef __STDC__
typedef void (*rx_bitset_iterator) (rx_Bitset, int member_index);
#else
typedef void (*rx_bitset_iterator) ();
#endif

/* Return the index of the word containing the Nth bit.
 */
#define rx_bitset_subset(N)  ((N) / RX_subset_bits)

/* Return the conveniently-sized supset containing the Nth bit.
 */
#define rx_bitset_subset_val(B,N)  ((B)[rx_bitset_subset(N)])


/* Genericly combine the word containing the Nth bit with a 1 bit mask
 * of the Nth bit position within that word.
 */
#define RX_bitset_access(B,N,OP) \
  ((B)[rx_bitset_subset(N)] OP rx_subset_singletons[(N) & RX_subset_mask])

#define RX_bitset_member(B,N)   RX_bitset_access(B, N, &)
#define RX_bitset_enjoin(B,N)   RX_bitset_access(B, N, |=)
#define RX_bitset_remove(B,N)   RX_bitset_access(B, N, &= ~)
#define RX_bitset_toggle(B,N)   RX_bitset_access(B, N, ^= )

/* How many words are needed for N bits?
 */
#define rx_bitset_numb_subsets(N) (((N) + RX_subset_bits - 1) / RX_subset_bits)

/* How much memory should be allocated for a bitset with N bits?
 */
#define rx_sizeof_bitset(N)	(rx_bitset_numb_subsets(N) * sizeof(RX_subset))


extern RX_subset rx_subset_singletons[];



#ifdef __STDC__
extern int rx_bitset_is_equal (int size, rx_Bitset a, rx_Bitset b);
extern int rx_bitset_is_subset (int size, rx_Bitset a, rx_Bitset b);
extern int rx_bitset_empty (int size, rx_Bitset set);
extern void rx_bitset_null (int size, rx_Bitset b);
extern void rx_bitset_universe (int size, rx_Bitset b);
extern void rx_bitset_complement (int size, rx_Bitset b);
extern void rx_bitset_assign (int size, rx_Bitset a, rx_Bitset b);
extern void rx_bitset_union (int size, rx_Bitset a, rx_Bitset b);
extern void rx_bitset_intersection (int size,
				    rx_Bitset a, rx_Bitset b);
extern void rx_bitset_difference (int size, rx_Bitset a, rx_Bitset b);
extern void rx_bitset_revdifference (int size,
				     rx_Bitset a, rx_Bitset b);
extern void rx_bitset_xor (int size, rx_Bitset a, rx_Bitset b);
extern unsigned long rx_bitset_hash (int size, rx_Bitset b);
extern int rx_bitset_population (int size, rx_Bitset a);

#else /* STDC */
extern int rx_bitset_is_equal ();
extern int rx_bitset_is_subset ();
extern int rx_bitset_empty ();
extern void rx_bitset_null ();
extern void rx_bitset_universe ();
extern void rx_bitset_complement ();
extern void rx_bitset_assign ();
extern void rx_bitset_union ();
extern void rx_bitset_intersection ();
extern void rx_bitset_difference ();
extern void rx_bitset_revdifference ();
extern void rx_bitset_xor ();
extern unsigned long rx_bitset_hash ();
extern int rx_bitset_population ();

#endif /* STDC */



#endif  /* RXBITSETH */

/* classes: h_files */

#ifndef RXHASHH
#define RXHASHH
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


#include "rxbitset.h"

/* giant inflatable hash trees */

struct rx_hash_item
{
  struct rx_hash_item * next_same_hash;
  struct rx_hash * table;
  unsigned long hash;
  void * data;
  void * binding;
};

struct rx_hash
{
  struct rx_hash * parent;
  int refs;
  RX_subset nested_p;
  void ** children[16];
};

struct rx_hash_rules;

/* rx_hash_eq should work like the == operator. */

#ifdef __STDC__
typedef int (*rx_hash_eq)(void *, void *);
typedef struct rx_hash * (*rx_alloc_hash)(struct rx_hash_rules *);
typedef void (*rx_free_hash)(struct rx_hash *,
			    struct rx_hash_rules *);
typedef struct rx_hash_item * (*rx_alloc_hash_item)(struct rx_hash_rules *,
						    void *);
typedef void (*rx_free_hash_item)(struct rx_hash_item *,
				 struct rx_hash_rules *);
typedef void (*rx_hash_freefn) (struct rx_hash_item * it);
#else
typedef int (*rx_hash_eq)();
typedef struct rx_hash * (*rx_alloc_hash)();
typedef void (*rx_free_hash)();
typedef struct rx_hash_item * (*rx_alloc_hash_item)();
typedef void (*rx_free_hash_item)();
typedef void (*rx_hash_freefn) ();
#endif

struct rx_hash_rules
{
  rx_hash_eq eq;
  rx_alloc_hash hash_alloc;
  rx_free_hash free_hash;
  rx_alloc_hash_item hash_item_alloc;
  rx_free_hash_item free_hash_item;
};


#ifdef __STDC__
extern struct rx_hash_item * rx_hash_find (struct rx_hash * table,
						   unsigned long hash,
						   void * value,
						   struct rx_hash_rules * rules);
extern struct rx_hash_item * rx_hash_store (struct rx_hash * table,
						    unsigned long hash,
						    void * value,
						    struct rx_hash_rules * rules);
extern void rx_hash_free (struct rx_hash_item * it, struct rx_hash_rules * rules);
extern void rx_free_hash_table (struct rx_hash * tab, rx_hash_freefn freefn,
					struct rx_hash_rules * rules);
extern int rx_count_hash_nodes (struct rx_hash * st);

#else /* STDC */
extern struct rx_hash_item * rx_hash_find ();
extern struct rx_hash_item * rx_hash_store ();
extern void rx_hash_free ();
extern void rx_free_hash_table ();
extern int rx_count_hash_nodes ();

#endif /* STDC */





#endif  /* RXHASHH */


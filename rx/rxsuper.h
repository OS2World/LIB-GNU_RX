/* classes: h_files */

#ifndef RXSUPERH
#define RXSUPERH

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

/*  lord	Sun May  7 12:40:17 1995	*/



#include "rxnfa.h"



/* This begins the description of the superstate NFA.
 *
 * The superstate NFA corresponds to the NFA in these ways:
 *
 * Superstate states correspond to sets of NFA states (nfa_states(SUPER)),
 *
 * Superstate edges correspond to NFA paths.
 *
 * The superstate has no epsilon transitions;
 * every edge has a character label, and a (possibly empty) side
 * effect label.   The side effect label corresponds to a list of
 * side effects that occur in the NFA.  These parts are referred
 * to as:   superedge_character(EDGE) and superedge_sides(EDGE).
 *
 * For a superstate edge EDGE starting in some superstate SUPER,
 * the following is true (in pseudo-notation :-):
 *
 *       exists DEST in nfa_states s.t. 
 *         exists nfaEDGE in nfa_edges s.t.
 *                 origin (nfaEDGE) == DEST
 *              && origin (nfaEDGE) is a member of nfa_states(SUPER)
 *              && exists PF in possible_futures(dest(nfaEDGE)) s.t.
 * 	                sides_of_possible_future (PF) == superedge_sides (EDGE)
 *
 * also:
 *
 *      let SUPER2 := superedge_destination(EDGE)
 *          nfa_states(SUPER2)
 *           == union of all nfa state sets S s.t.
 *                          exists PF in possible_futures(dest(nfaEDGE)) s.t.
 * 	                       sides_of_possible_future (PF) == superedge_sides (EDGE)
 *                          && S == dests_of_possible_future (PF) }
 *
 * Or in english, every superstate is a set of nfa states.  A given
 * character and a superstate implies many transitions in the NFA --
 * those that begin with an edge labeled with that character from a
 * state in the set corresponding to the superstate.
 * 
 * The destinations of those transitions each have a set of possible
 * futures.  A possible future is a list of side effects and a set of
 * destination NFA states.  Two sets of possible futures can be
 * `merged' by combining all pairs of possible futures that have the
 * same side effects.  A pair is combined by creating a new future
 * with the same side effect but the union of the two destination sets.
 * In this way, all the possible futures suggested by a superstate
 * and a character can be merged into a set of possible futures where
 * no two elements of the set have the same set of side effects.
 *
 * The destination of a possible future, being a set of NFA states, 
 * corresponds to a supernfa state.  So, the merged set of possible
 * futures we just created can serve as a set of edges in the
 * supernfa.
 *
 * The representation of the superstate nfa and the nfa is critical.
 * The nfa has to be compact, but has to facilitate the rapid
 * computation of missing superstates.  The superstate nfa has to 
 * be fast to interpret, lazilly constructed, and bounded in space.
 *
 * To facilitate interpretation, the superstate data structures are 
 * peppered with `instruction frames'.  There is an instruction set
 * defined below which matchers using the supernfa must be able to
 * interpret.
 *
 * We'd like to make it possible but not mandatory to use code
 * addresses to represent instructions (c.f. gcc's computed goto).
 * Therefore, we define an enumerated type of opcodes, and when
 * writing one of these instructions into a data structure, use
 * the opcode as an index into a table of instruction values.
 * 
 * Below are the opcodes that occur in the superstate nfa.
 *
 * The descriptions of the opcodes refer to data structures that are
 * described further below. 
 */

enum rx_opcode
{
  /* 
   * BACKTRACK_POINT is invoked when a character transition in 
   * a superstate leads to more than one edge.  In that case,
   * the edges have to be explored independently using a backtracking
   * strategy.
   *
   * A BACKTRACK_POINT instruction is stored in a superstate's 
   * transition table for some character when it is known that that
   * character crosses more than one edge.  On encountering this
   * instruction, the matcher saves enough state to backtrack to this
   * point later in the match.
   */
  rx_backtrack_point = 0,	/* data is (struct transition_class *) */

  /* 
   * RX_DO_SIDE_EFFECTS evaluates the side effects of an epsilon path.
   * There is one occurence of this instruction per rx_distinct_future.
   * This instruction is skipped if a rx_distinct_future has no side effects.
   */
  rx_do_side_effects = rx_backtrack_point + 1,

  /* data is (struct rx_distinct_future *) */

  /* 
   * RX_CACHE_MISS instructions are stored in rx_distinct_futures whose
   * destination superstate has been reclaimed (or was never built).
   * It recomputes the destination superstate.
   * RX_CACHE_MISS is also stored in a superstate transition table before
   * any of its edges have been built.
   */
  rx_cache_miss = rx_do_side_effects + 1,
  /* data is (struct rx_distinct_future *) */

  /* 
   * RX_NEXT_CHAR is called to consume the next character and take the
   * corresponding transition.  This is the only instruction that uses 
   * the DATA field of the instruction frame instead of DATA_2.
   * The comments about rx_inx explain this further.
   */
  rx_next_char = rx_cache_miss + 1, /* data is (struct superstate *) */

  /* RX_BACKTRACK indicates that a transition fails.  Don't
   * confuse this with rx_backtrack_point.
   */
  rx_backtrack = rx_next_char + 1, /* no data */

  /* 
   * RX_ERROR_INX is stored only in places that should never be executed.
   */
  rx_error_inx = rx_backtrack + 1, /* Not supposed to occur. */

  rx_num_instructions = rx_error_inx + 1
};

/* The heart of the matcher is a `word-code-interpreter' 
 * (like a byte-code interpreter, except that instructions
 * are a full word wide).
 *
 * Instructions are not stored in a vector of code, instead,
 * they are scattered throughout the data structures built
 * by the regexp compiler and the matcher.  One word-code instruction,
 * together with the arguments to that instruction, constitute
 * an instruction frame (struct rx_inx).
 *
 * This structure type is padded by hand to a power of 2 because
 * in one of the dominant cases, we dispatch by indexing a table
 * of instruction frames.  If that indexing can be accomplished
 * by just a shift of the index, we're happy.
 *
 * Instructions take at most one argument, but there are two
 * slots in an instruction frame that might hold that argument.
 * These are called data and data_2.  The data slot is only
 * used for one instruction (RX_NEXT_CHAR).  For all other 
 * instructions, data should be set to 0.
 *
 * RX_NEXT_CHAR is the most important instruction by far.
 * By reserving the data field for its exclusive use, 
 * instruction dispatch is sped up in that case.  There is
 * no need to fetch both the instruction and the data,
 * only the data is needed.  In other words, a `cycle' begins
 * by fetching the field data.  If that is non-0, then it must
 * be the destination state of a next_char transition, so
 * make that value the current state, advance the match position
 * by one character, and start a new cycle.  On the other hand,
 * if data is 0, fetch the instruction and do a more complicated
 * dispatch on that.
 */

struct rx_inx 
{
  void * data;
  void * data_2;
  void * inx;
  void * fnord;
};

#ifndef RX_TAIL_ARRAY
#define RX_TAIL_ARRAY  1
#endif

/* A superstate corresponds to a set of nfa states.  Those sets are
 * represented by STRUCT RX_SUPERSET.  The constructors
 * guarantee that only one (shared) structure is created for a given set.
 */
struct rx_superset
{
  int is_final;
  int refs;			/* This is a reference counted structure. */

  /* We keep these sets in a cache because (in an unpredictable way),
   * the same set is often created again and again.  But that is also
   * problematic -- compatibility with POSIX and GNU regex requires
   * that we not be able to tell when a program discards a particular
   * NFA (thus invalidating the supersets created from it).
   *
   * But when a cache hit appears to occur, we will have in hand the
   * nfa for which it may have happened.  That is why every nfa is given
   * its own sequence number.  On a cache hit, the cache is validated
   * by comparing the nfa sequence number to this field:
   */
  int id;

  struct rx_nfa_state * car;	/* May or may not be a valid addr. */
  struct rx_superset * cdr;

  /* If the corresponding superstate exists: */
  struct rx_superstate * superstate;


  /* There is another bookkeeping problem.  It is expensive to 
   * compute the starting nfa state set for an nfa.  So, once computed,
   * it is cached in the `struct rx'.
   *
   * But, the state set can be flushed from the superstate cache.
   * When that happens, we can't know if the corresponding `struct rx'
   * is still alive or if it has been freed or re-used by the program.
   * So, the cached pointer to this set in a struct rx might be invalid
   * and we need a way to validate it.
   *
   * Fortunately, even if this set is flushed from the cache, it is
   * not freed.  It just goes on the free-list of supersets.
   * So we can still examine it.  
   *
   * So to validate a starting set memo, check to see if the
   * starts_for field still points back to the struct rx in question,
   * and if the ID matches the rx sequence number.
   */
  struct rx * starts_for;

  /* This is used to link into a hash bucket so these objects can
   * be `hash-consed'.
   */
  struct rx_hash_item hash_item;
};

#define rx_protect_superset(RX,CON) (++(CON)->refs)

/* The terminology may be confusing (rename this structure?).
 * Every character occurs in at most one rx_super_edge per super-state.
 * But, that structure might have more than one option, indicating a point
 * of non-determinism. 
 *
 * In other words, this structure holds a list of superstate edges
 * sharing a common starting state and character label.  The edges
 * are in the field OPTIONS.  All superstate edges sharing the same
 * starting state and character are in this list.
 */
struct rx_super_edge
{
  struct rx_super_edge *next;
  struct rx_inx rx_backtrack_frame;
  int cset_size;
  rx_Bitset cset;
  struct rx_distinct_future *options;
};

/* A superstate is a set of nfa states (RX_SUPERSET) along
 * with a transition table.  Superstates are built on demand and reclaimed
 * without warning.  To protect a superstate from this ghastly fate,
 * use LOCK_SUPERSTATE. 
 */
struct rx_superstate
{
  int rx_id;			/* c.f. the id field of rx_superset */
  int locks;			/* protection from reclamation */

  /* Within a superstate cache, all the superstates are kept in a big
   * queue.  The tail of the queue is the state most likely to be
   * reclaimed.  The *recyclable fields hold the queue position of 
   * this state.
   */
  struct rx_superstate * next_recyclable;
  struct rx_superstate * prev_recyclable;

  /* The supernfa edges that exist in the cache and that have
   * this state as their destination are kept in this list:
   */
  struct rx_distinct_future * transition_refs;

  /* The list of nfa states corresponding to this superstate: */
  struct rx_superset * contents;

  /* The list of edges in the cache beginning from this state. */
  struct rx_super_edge * edges;

  /* A tail of the recyclable queue is marked as semifree.  A semifree
   * state has no incoming next_char transitions -- any transition
   * into a semifree state causes a complex dispatch with the side
   * effect of rescuing the state from its semifree state into a 
   * fully free state at the head of the queue.
   *
   * An alternative to this might be to make next_char more expensive,
   * and to move a state to the head of the recyclable queue whenever
   * it is entered.  That way, popular states would never be recycled.
   *
   * But unilaterally making next_char more expensive actually loses.
   * So, incoming transitions are only made expensive for states near
   * the tail of the recyclable queue.  The more cache contention
   * there is, the more frequently a state will have to prove itself
   * and be moved back to the front of the queue.  If there is less 
   * contention, then popular states just aggregate in the front of 
   * the queue and stay there.
   */
  int is_semifree;


  /* This keeps track of the size of the transition table for this
   * state.  There is a half-hearted attempt to support variable sized
   * superstates.
   */
  int trans_size;

  /* Indexed by characters... */
  struct rx_inx transitions[RX_TAIL_ARRAY];
};


/* A list of distinct futures define the edges that leave from a 
 * given superstate on a given character.  c.f. rx_super_edge.
 */
struct rx_distinct_future
{
  struct rx_distinct_future * next_same_super_edge[2];
  struct rx_distinct_future * next_same_dest;
  struct rx_distinct_future * prev_same_dest;
  struct rx_superstate * present;	/* source state */
  struct rx_superstate * future;	/* destination state */
  struct rx_super_edge * edge;


  /* The future_frame holds the instruction that should be executed
   * after all the side effects are done, when it is time to complete
   * the transition to the next state.
   *
   * Normally this is a next_char instruction, but it may be a
   * cache_miss instruction as well, depending on whether or not
   * the superstate is in the cache and semifree.
   * 
   * If this is the only future for a given superstate/char, and
   * if there are no side effects to be performed, this frame is
   * not used (directly) at all.  Instead, its contents are copied
   * into the transition table of the starting state of this dist. future
   * (a sort of goto elimination).
   */
  struct rx_inx future_frame;

  struct rx_inx side_effects_frame;
  struct rx_se_list * effects;
};

#define rx_lock_superstate(R,S)  ((S)->locks++)
#define rx_unlock_superstate(R,S) (--(S)->locks)


struct rx_cache;

#ifdef __STDC__
typedef void (*rx_morecore_fn)(struct rx_cache *);
#else
typedef void (*rx_morecore_fn)();
#endif

struct rx_cache
{
  struct rx_hash_rules superset_hash_rules;

  struct rx_superstate * lru_superstate;
  struct rx_superstate * semifree_superstate;

  struct rx_superset * empty_superset;

  int superstates;
  int semifree_superstates;
  int hits;
  int misses;

  int bytes_allowed;
  int bytes_used;

  int local_cset_size;
  void ** instruction_table;

  struct rx_hash superset_table;
};

extern struct rx_cache * rx_default_cache;


#ifdef __STDC__
extern void rx_release_superset (struct rx *rx,
				 struct rx_superset *set);
extern struct rx_superset * rx_superset_cons (struct rx * rx,
					      struct rx_nfa_state *car, struct rx_superset *cdr);
extern struct rx_superset * rx_superstate_eclosure_union
(struct rx * rx, struct rx_superset *set, struct rx_nfa_state_set *ecl) ;
extern struct rx_superstate * rx_superstate (struct rx *rx,
					     struct rx_superset *set);
extern struct rx_inx * rx_handle_cache_miss
(struct rx *rx, struct rx_superstate *super, unsigned char chr, void *data) ;

#else /* STDC */
extern void rx_release_superset ();
extern struct rx_superset * rx_superset_cons ();
extern struct rx_superset * rx_superstate_eclosure_union
extern struct rx_superstate * rx_superstate ();
extern struct rx_inx * rx_handle_cache_miss

#endif /* STDC */




#endif  /* RXSUPERH */

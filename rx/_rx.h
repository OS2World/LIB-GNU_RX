/* classes: h_files */

#ifndef _RXH
#define _RXH
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



#include <sys/types.h>
#include "rxhash.h"
#include "rxcset.h"



struct rx_cache;
struct rx_superset;
struct rx;
struct rx_se_list;



/* Suppose that from some NFA state and next character, more than one
 * path through side-effect edges is possible.  In what order should
 * the paths be tried?  A function of type rx_se_list_order answers
 * that question.  It compares two lists of side effects, and says
 * which list comes first.
 */
 
#ifdef __STDC__
typedef int (*rx_se_list_order) (struct rx *,
				 struct rx_se_list *, 
				 struct rx_se_list *);
#else
typedef int (*rx_se_list_order) ();
#endif



/* Struct RX holds an NFA and cache state for the corresponding super NFA.
 */
struct rx
{
  /* The compiler assigns a unique id to every pattern.
   * Like sequence numbers in X, there is a subtle bug here
   * if you use Rx in a system that runs for a long time.
   * But, because of the way the caches work out, it is almost
   * impossible to trigger the Rx version of this bug.
   *
   * The id is used to validate superstates found in a cache
   * of superstates.  It isn't sufficient to let a superstate
   * point back to the rx for which it was compiled -- the caller
   * may be re-using a `struct rx' in which case the superstate
   * is not really valid.  So instead, superstates are validated
   * by checking the sequence number of the pattern for which
   * they were built.
   */
  int rx_id;

  /* This is memory mgt. state for superstates.  This may be 
   * shared by more than one struct rx.
   */
  struct rx_cache * cache;

  /* Every nfa defines the size of its own character set. 
   * Each superstate has an array of this size, with each element
   * a `struct rx_inx'.  So, don't make this number too large.
   * In particular, don't make it 2^16.
   */
  int local_cset_size;

  /* Lists of side effects as stored in the NFA are `hash consed'..meaning
   * that lists with the same elements are ==.  During compilation, 
   * this table facilitates hash-consing.
   */
  struct rx_hash se_list_memo;

  /* Lists of NFA states are also hashed. 
   */
  struct rx_hash set_list_memo;



  /* The compiler and matcher must build a number of instruction frames.
   * The format of these frames is fixed (c.f. struct rx_inx).  The values
   * of the instruction opcodes is not fixed.
   *
   * An enumerated type (enum rx_opcode) defines the set of instructions
   * that the compiler or matcher might generate.  When filling an instruction
   * frame, the INX field is found by indexing this instruction table
   * with an opcode:
   */
  void ** instruction_table;

  /* The list of all states in an NFA.
   * The NEXT field of NFA states links this list.
   */
  struct rx_nfa_state *nfa_states;
  struct rx_nfa_state *start_nfa_states;
  struct rx_superset * start_set;

  /* This orders the search through super-nfa paths.
   * See the comment near the typedef of rx_se_list_order.
   */
  rx_se_list_order se_list_cmp;

  int next_nfa_id;
};



/* Type for byte offsets within the string.  POSIX mandates this.  */
typedef int regoff_t;

/* This is the structure we store register match data in.  See
 * regex.texinfo for a full description of what registers match.
 */
struct re_registers
{
  unsigned num_regs;
  regoff_t *start;
  regoff_t *end;
};

/* If `regs_allocated' is REGS_UNALLOCATED in the pattern buffer,
 * `re_match_2' returns information about at least this many registers
 * the first time a `regs' structure is passed. 
 *
 * Also, this is the greatest number of backreferenced subexpressions
 * allowed in a pattern being matched without caller-supplied registers.
 */
#ifndef RE_NREGS
#define RE_NREGS 30
#endif


#ifndef emacs
#define CHARBITS 8
#define CHAR_SET_SIZE (1 << CHARBITS)
#define Sword 1
#define SYNTAX(c) re_syntax_table[c]
extern char re_syntax_table[CHAR_SET_SIZE];
#endif


/* Should we use malloc or alloca?  If REGEX_MALLOC is not defined, we
 * use `alloca' instead of `malloc' for the backtracking stack.
 *
 * Emacs will die miserably if we don't do this.
 */

#ifdef REGEX_MALLOC
#define REGEX_ALLOCATE malloc
#else /* not REGEX_MALLOC  */
#define REGEX_ALLOCATE alloca
#endif /* not REGEX_MALLOC */

#undef MAX
#undef MIN
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
extern void * rx_id_instruction_table[];
extern struct rx_cache * rx_default_cache;


#ifdef __STDC__

#else /* STDC */

#endif /* STDC */


#endif  /* _RXH */

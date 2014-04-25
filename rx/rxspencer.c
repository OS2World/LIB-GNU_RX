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
#include "rxspencer.h"



static int rx_ms = 0;
static int rx_fs = 0;

static char * silly_hack_2 = 0;

#ifdef __STDC__
struct rx_solutions *
rx_make_solutions (struct rx_registers * regs,
		   struct rx_unfaniverse * verse,
		   struct rexp_node * expression,
		   struct rexp_node ** subexps,
		   int cset_size,
		   int start, int end,
		   rx_vmfn vmfn, rx_contextfn contextfn,
		   void * closure)
#else
struct rx_solutions *
rx_make_solutions (regs, verse, expression, subexps, cset_size, 
		   start, end, vmfn, contextfn, closure)
     struct rx_registers * regs;
     struct rx_unfaniverse * verse;
     struct rexp_node * expression;
     struct rexp_node ** subexps;
     int cset_size;
     int start;
     int end;
     rx_vmfn vmfn;
     rx_contextfn contextfn;
     void * closure;
#endif
{
  struct rx_solutions * solns;
  unsigned char * burst;
  int len;
  int offset;

  ++rx_ms;
  if (silly_hack_2)
    {
      solns = (struct rx_solutions *)silly_hack_2;
      silly_hack_2 = 0;
    }
  else
    solns = (struct rx_solutions *)malloc (sizeof (*solns));
  if (!solns)
    return 0;
  bzero (solns, sizeof (*solns));

  solns->step = 0;
  solns->cset_size = cset_size;
  solns->subexps = subexps;
  solns->exp = expression;
  rx_save_rexp (expression);
  solns->verse = verse;
  solns->regs = regs;
  solns->start = start;
  solns->end = end;
  solns->vmfn = vmfn;
  solns->contextfn = contextfn;
  solns->closure = closure;
  solns->current_pos = start;
  solns->interval_x = 0;

  if (!solns->exp->observed)
    {
      solns->dfa = rx_unfa (verse, expression, cset_size);
      if (!solns->dfa)
	goto err_return;
      rx_init_system (&solns->match_engine, solns->dfa->nfa);

      {
	struct rx * rx;
	rx = solns->match_engine.rx;
#if 0
	if (   rx->start_set
	    && (rx->start_set->starts_for == rx)
	    && (rx->start_set->id == rx->rx_id)
	    && rx->start_set->superstate
	    && (rx->start_set->superstate->rx_id == rx->rx_id))
	  {
	    solns->match_engine.state = solns->match_engine.rx->start_set->superstate;
	    rx_lock_superstate (solns->match_engine.rx, solns->match_engine.state);
	  }
	else
	  ;
#endif
	if (rx_yes != rx_start_superstate (&solns->match_engine))
	  goto err_return;
      }
    }
  else
    {
      struct rexp_node * simplified;
      int status;
      status = rx_simple_rexp (&simplified, cset_size, solns->exp, subexps);
      if (status)
	goto err_return;
      solns->dfa = rx_unfa (verse, simplified, cset_size);
      if (!solns->dfa)
	{
	  rx_free_rexp (simplified);
	  goto err_return;
	}
      rx_init_system (&solns->match_engine, solns->dfa->nfa);
      if (rx_yes != rx_start_superstate (&solns->match_engine))
	goto err_return;
      rx_free_rexp (simplified);
    }
  return solns;

 err_return:
  rx_free_rexp (solns->exp);
  free (solns);
  return 0;
}



#ifdef __STDC__
void
rx_free_solutions (struct rx_solutions * solns)
#else
void
rx_free_solutions (solns)
     struct rx_solutions * solns;
#endif
{
  if (!solns)
    return;

  ++rx_fs;
  if (solns->left)
    {
      rx_free_solutions (solns->left);
      solns->left = 0;
    }

  if (solns->right)
    {
      rx_free_solutions (solns->right);
      solns->right = 0;
    }

  if (solns->dfa)
    {
      rx_free_unfa (solns->dfa);
      solns->dfa = 0;
    }

  rx_terminate_system (&solns->match_engine);

  if (solns->exp)
    {
      rx_free_rexp (solns->exp);
      solns->exp = 0;
    }

  if (!silly_hack_2)
    silly_hack_2 = (char *)solns;
  else
    free (solns);
}


/* This doesn't deal with resumable rx_maybe right. */
enum rx_answers
rx_soluntion_fit_p (solns)
     struct rx_solutions * solns;
{
  unsigned char * burst;
  int burst_addr;
  int burst_len;
  int burst_end_addr;
  int rel_pos_in_burst;
  enum rx_answers vmstat;
	  
  
 next_burst:
  vmstat = solns->vmfn (solns->closure,
			&burst, &burst_len, &burst_addr,
			solns->current_pos, solns->end,
			solns->current_pos);

  if (vmstat != rx_yes)
    return vmstat;

  rel_pos_in_burst = solns->current_pos - burst_addr;
  burst_end_addr = burst_addr + burst_len;

  if (burst_end_addr >= solns->end)
    {
      enum rx_answers fit_status;
      fit_status = rx_fit_p (&solns->match_engine,
			     burst + rel_pos_in_burst,
			     solns->end - solns->current_pos);
      return fit_status;
    }
  else
    {
      enum rx_answers fit_status;
      fit_status = rx_advance (&solns->match_engine,
			       burst + rel_pos_in_burst,
			       burst_len - rel_pos_in_burst);
      if (fit_status != rx_yes)
	{
	  return fit_status;
	}
      else
	{
	  solns->current_pos += burst_len - rel_pos_in_burst;
	  goto next_burst;
	}
    }
}

#ifdef __STDC__
enum rx_answers
rx_next_solution (struct rx_solutions * solns)
#else
enum rx_answers
rx_next_solution (solns)
     struct rx_solutions * solns;
#endif
{
  if (!solns)
    return rx_bogus;

  if (!solns->exp)
    {
      if (solns->step != 0)
	return rx_no;
      else
	{
	  solns->step = 1;
	  return (solns->start == solns->end
		  ? rx_yes
		  : rx_no);
	}
    }
  else if (   (solns->exp->len >= 0)
	   && (solns->exp->len != (solns->end - solns->start)))
    {
      return rx_no;
    }
  else if (!solns->exp->observed)
    {
      if (solns->step != 0)
	return rx_no;
      else
	{
	  enum rx_answers ans;
	  ans = rx_soluntion_fit_p (solns);
	  solns->step = -1;
	  return ans;
	}
    }
  else if (solns->exp->observed)
    {
      enum rx_answers fit_p;
      switch (solns->step)
	{
	case -1:
	  return rx_no;
	case 0:
	  fit_p = rx_soluntion_fit_p (solns);
	  switch (fit_p)
	    {
	    case rx_no:
	    case rx_maybe:
	      solns->step = -1;
	      return rx_no;
	    case rx_yes:
	      solns->step = 1;
	      goto resolve_fit;
	    case rx_bogus:
	    default:
	      solns->step = -1;
	      return fit_p;
	    }

	default:
	resolve_fit:
	  switch (solns->exp->type)
	    {
	    case r_cset:
	      solns->step = -1;
	      return rx_bogus;
	      
	    case r_parens:
	      {
		enum rx_answers paren_stat;
		switch (solns->step)
		  {
		  case 1:

		    if (   !solns->exp->params.pair.left
			|| !solns->exp->params.pair.left->observed)
		      {
			solns->regs[solns->exp->params.intval].rm_so = solns->start;
			solns->regs[solns->exp->params.intval].rm_eo = solns->end;
			solns->step = -1;
			return rx_yes;
		      }
		    else
		      {
			solns->left = rx_make_solutions (solns->regs,
							 solns->verse,
							 solns->exp->params.pair.left,
							 solns->subexps,
							 solns->cset_size,
							 solns->start,
							 solns->end,
							 solns->vmfn,
							 solns->contextfn,
							 solns->closure);
			if (!solns->left)
			  {
			    solns->step = -1;
			    return rx_bogus;
			  }
		      }
		    solns->step = 2;
		    /* fall through */

		  case 2:
		    solns->regs[solns->exp->params.intval].rm_so = -1;
		    solns->regs[solns->exp->params.intval].rm_eo = -1;

		    paren_stat = rx_next_solution (solns->left);

		    if (paren_stat == rx_yes)
		      {
			solns->regs[solns->exp->params.intval].rm_so = solns->start;
			solns->regs[solns->exp->params.intval].rm_eo = solns->end;
			return rx_yes;
		      }
		    else if (paren_stat != rx_maybe)
		      {
			solns->step = -1;
			rx_free_solutions (solns->left);
			solns->left = 0;
			return paren_stat;
		      }
		    else /* paren_stat == rx_maybe */
		      return paren_stat;
		  }
	      }


	    case r_opt:
	      {
		enum rx_answers opt_stat;
		switch (solns->step)
		  {
		  case 1:
		    solns->left = rx_make_solutions (solns->regs,
						     solns->verse,
						     solns->exp->params.pair.left,
						     solns->subexps,
						     solns->cset_size,
						     solns->start,
						     solns->end,
						     solns->vmfn,
						     solns->contextfn,	
						     solns->closure);
		    if (!solns->left)
		      {
			solns->step = -1;
			return rx_bogus;
		      }
		    solns->step = 2;
		    /* fall through */
		    
		  case 2:
		    opt_stat = rx_next_solution (solns->left);

		    if (opt_stat == rx_yes)
		      return rx_yes;
		    else if (opt_stat != rx_maybe)
		      {
			solns->step = -1;
			rx_free_solutions (solns->left);
			solns->left = 0;
			return ((solns->start == solns->end)
				? rx_yes
				: rx_no);
		      }
		    else /* opt_stat == rx_maybe */
		      return opt_stat;

		  }
	     }

	    case r_alternate:
	      {
		enum rx_answers alt_stat;
		switch (solns->step)
		  {
		  case 1:
		    solns->left = rx_make_solutions (solns->regs,
						     solns->verse,
						     solns->exp->params.pair.left,
						     solns->subexps,
						     solns->cset_size,
						     solns->start,
						     solns->end,
						     solns->vmfn,
						     solns->contextfn,
						     solns->closure);
		    if (!solns->left)
		      {
			solns->step = -1;
			return rx_bogus;
		      }
		    solns->step = 2;
		    /* fall through */
		    
		  case 2:
		    alt_stat = rx_next_solution (solns->left);

		    if (   (alt_stat == rx_yes)
			|| (alt_stat == rx_maybe))
		      return alt_stat;
		    else 
		      {
			solns->step = 3;
			rx_free_solutions (solns->left);
			solns->left = 0;
			/* fall through */
		      }

		  case 3:
		    solns->right = rx_make_solutions (solns->regs,
						      solns->verse,
						      solns->exp->params.pair.right,
						      solns->subexps,
						      solns->cset_size,
						      solns->start,
						      solns->end,
						      solns->vmfn,
						      solns->contextfn,
						      solns->closure);
		    if (!solns->right)
		      {
			solns->step = -1;
			return rx_bogus;
		      }
		    solns->step = 4;
		    /* fall through */
		    
		  case 4:
		    alt_stat = rx_next_solution (solns->right);

		    if (   (alt_stat == rx_yes)
			|| (alt_stat == rx_maybe))
		      return alt_stat;
		    else 
		      {
			solns->step = -1;
			rx_free_solutions (solns->right);
			solns->right = 0;
			return alt_stat;
		      }
		  }
	     }

	    case r_concat:
	      {
		switch (solns->step)
		  {
		    enum rx_answers concat_stat;
		  case 1:
		    solns->split_guess = solns->end;

		  concat_split_guess_loop:
		    solns->left = rx_make_solutions (solns->regs,
						     solns->verse,
						     solns->exp->params.pair.left,
						     solns->subexps,
						     solns->cset_size,
						     solns->start,
						     solns->split_guess,
						     solns->vmfn,
						     solns->contextfn,
						     solns->closure);
		    if (!solns->left)
		      {
			solns->step = -1;
			return rx_bogus;
		      }
		    solns->step = 2;

		  case 2:
		  concat_try_next_left_match:

		    concat_stat = rx_next_solution (solns->left);
		    if (concat_stat == rx_maybe)
		      return concat_stat;
		    else if (concat_stat != rx_yes)
		      {
			rx_free_solutions (solns->left);
			rx_free_solutions (solns->right);
			solns->left = solns->right = 0;
			solns->split_guess -= 1;
			if (solns->split_guess >= solns->start)
			  goto concat_split_guess_loop;
			else
			  {
			    solns->step = -1;
			    return concat_stat;
			  }
		      }
		    else
		      {
			solns->step = 3;
			/* fall through */
		      }

		  case 3:
		    solns->right = rx_make_solutions (solns->regs,
						      solns->verse,
						      solns->exp->params.pair.right,
						      solns->subexps,
						      solns->cset_size,
						      solns->split_guess,
						      solns->end,
						      solns->vmfn,
						      solns->contextfn,
						      solns->closure);
		    if (!solns->right)
		      {
			rx_free_solutions (solns->left);
			solns->left = 0;
			solns->step = -1;
			return rx_bogus;
		      }

		    solns->step = 4;
		    /* fall through */

		  case 4:
		  concat_try_next_right_match:

		    concat_stat = rx_next_solution (solns->right);
		    if ((concat_stat == rx_maybe) || (concat_stat == rx_yes))
		      return concat_stat;
		    else if (concat_stat == rx_no)
		      {
			rx_free_solutions (solns->right);
			solns->right = 0;
			solns->step = 2;
			goto concat_try_next_left_match;
		      }
		    else /*  concat_stat == rx_bogus */
		      {
			rx_free_solutions (solns->left);
			solns->left = 0;
			rx_free_solutions (solns->right);
			solns->right = 0;
			solns->step = -1;
			return concat_stat;
		      }
		  }
	      }


	    case r_plus:
	    case r_star:
	      {
		switch (solns->step)
		  {
		    enum rx_answers star_stat;
		  case 1:
		    solns->split_guess = solns->end;

		  star_split_guess_loop:
		    solns->left = rx_make_solutions (solns->regs,
						     solns->verse,
						     solns->exp->params.pair.left,
						     solns->subexps,
						     solns->cset_size,
						     solns->start,
						     solns->split_guess,
						     solns->vmfn,
						     solns->contextfn,
						     solns->closure);
		    if (!solns->left)
		      {
			solns->step = -1;
			return rx_bogus;
		      }
		    solns->step = 2;

		  case 2:
		  star_try_next_left_match:

		    star_stat = rx_next_solution (solns->left);
		    if (star_stat == rx_maybe)
		      return star_stat;
		    else if (star_stat != rx_yes)
		      {
			rx_free_solutions (solns->left);
			rx_free_solutions (solns->right);
			solns->left = solns->right = 0;
			solns->split_guess -= 1;
			if (solns->split_guess >= solns->start)
			  goto star_split_guess_loop;
			else
			  {
			    solns->step = -1;

			    if (   (solns->exp->type == r_star)
				&& (solns->split_guess == solns->end))
			      return rx_yes;
			    else
			      return star_stat;
			  }
		      }
		    else
		      {
			solns->step = 3;
			/* fall through */
		      }


		    if (solns->split_guess == solns->end)
		      return rx_yes;
		    
		  case 3:
		    solns->right = rx_make_solutions (solns->regs,
						      solns->verse,
						      solns->exp,
						      solns->subexps,
						      solns->cset_size,
						      solns->split_guess,
						      solns->end,
						      solns->vmfn,
						      solns->contextfn,
						      solns->closure);
		    if (!solns->right)
		      {
			rx_free_solutions (solns->left);
			solns->left = 0;
			solns->step = -1;
			return rx_bogus;
		      }

		    solns->step = 4;
		    /* fall through */

		  case 4:
		  star_try_next_right_match:
		    
		    star_stat = rx_next_solution (solns->right);
		    if ((star_stat == rx_maybe) || (star_stat == rx_yes))
		      return star_stat;
		    else if (star_stat == rx_no)
		      {
			rx_free_solutions (solns->right);
			solns->right = 0;
			solns->step = 2;
			goto star_try_next_left_match;
		      }
		    else /*  star_stat == rx_bogus */
		      {
			rx_free_solutions (solns->left);
			solns->left = 0;
			rx_free_solutions (solns->right);
			solns->right = 0;
			solns->step = -1;
			return star_stat;
		      }
		  }
	      }


	    case r_context:
	      {
		solns->step = -1;
		return solns->contextfn (solns->closure,
					 solns->exp->params.intval,
					 solns->start, solns->end,
					 solns->regs);
	      }

	    case r_interval:
	      {
		switch (solns->step)
		  {
		    enum rx_answers interval_stat;

		  case 1:
		    if (solns->interval_x >= solns->exp->params.intval2)
		      {
			solns->step = -1;
			return rx_no;
		      }
		    
		    solns->split_guess = solns->end;
		    
		  interval_split_guess_loop:
		    solns->left = rx_make_solutions (solns->regs,
						     solns->verse,
						     solns->exp->params.pair.left,
						     solns->subexps,
						     solns->cset_size,
						     solns->start,
						     solns->split_guess,
						     solns->vmfn,
						     solns->contextfn,
						     solns->closure);
		    if (!solns->left)
		      {
			solns->step = -1;
			return rx_bogus;
		      }
		    solns->step = 2;

		  case 2:
		  interval_try_next_left_match:

		    interval_stat = rx_next_solution (solns->left);
		    if (interval_stat == rx_maybe)
		      return interval_stat;
		    else if (interval_stat != rx_yes)
		      {
			rx_free_solutions (solns->left);
			rx_free_solutions (solns->right);
			solns->left = solns->right = 0;
			solns->split_guess -= 1;
			if (solns->split_guess >= solns->start)
			  goto interval_split_guess_loop;
			else
			  {
			    solns->step = -1;
			    return ((   (interval_stat == rx_no)
				     && (   solns->interval_x
					 == (solns->exp->params.intval2)))
				    ? rx_yes
				    : rx_no);
			  }
		      }
		    else
		      {
			solns->step = 3;
			/* fall through */
		      }


		    if (   (solns->split_guess == solns->end)
			&& (solns->interval_x >= (solns->exp->params.intval - 1)))
		      return rx_yes;
		    
		  case 3:
		    solns->right = rx_make_solutions (solns->regs,
						      solns->verse,
						      solns->exp,
						      solns->subexps,
						      solns->cset_size,
						      solns->split_guess,
						      solns->end,
						      solns->vmfn,
						      solns->contextfn,
						      solns->closure);
		    solns->right->interval_x = solns->interval_x + 1;
		    if (!solns->right)
		      {
			rx_free_solutions (solns->left);
			solns->left = 0;
			solns->step = -1;
			return rx_bogus;
		      }

		    solns->step = 4;
		    /* fall through */

		  case 4:
		  interval_try_next_right_match:
		    
		    interval_stat = rx_next_solution (solns->right);
		    if ((interval_stat == rx_maybe) || (interval_stat == rx_yes))
		      return interval_stat;
		    else if (interval_stat == rx_no)
		      {
			rx_free_solutions (solns->right);
			solns->right = 0;
			solns->step = 2;
			goto interval_try_next_left_match;
		      }
		    else /*  interval_stat == rx_bogus */
		      {
			rx_free_solutions (solns->left);
			solns->left = 0;
			rx_free_solutions (solns->right);
			solns->right = 0;
			solns->step = -1;
			return interval_stat;
		      }
		  }
	      }
	    }
	}
      return rx_bogus;
    }
}

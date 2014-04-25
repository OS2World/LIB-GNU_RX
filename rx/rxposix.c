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
#include "rxposix.h"
#include "rxgnucomp.h"
#include "rxbasic.h"
#include "rxsimp.h"

/* regcomp takes a regular expression as a string and compiles it.
 *
 * PATTERN is the address of the pattern string.
 *
 * CFLAGS is a series of bits which affect compilation.
 *
 *   If REG_EXTENDED is set, we use POSIX extended syntax; otherwise, we
 *   use POSIX basic syntax.
 *
 *   If REG_NEWLINE is set, then . and [^...] don't match newline.
 *   Also, regexec will try a match beginning after every newline.
 *
 *   If REG_ICASE is set, then we considers upper- and lowercase
 *   versions of letters to be equivalent when matching.
 *
 *   If REG_NOSUB is set, then when PREG is passed to regexec, that
 *   routine will report only success or failure, and nothing about the
 *   registers.
 *
 * It returns 0 if it succeeds, nonzero if it doesn't.  (See regex.h for
 * the return codes and their meanings.)  
 */


#ifdef __STDC__
int
regcomp (regex_t * preg, const char * pattern, int cflags)
#else
int
regcomp (preg, pattern, cflags)
    regex_t * preg;
    const char * pattern;
    int cflags;
#endif
{
  reg_errcode_t ret;
  unsigned int syntax;
  unsigned char xlate[256];

  bzero (preg, sizeof (*preg));
  syntax = ((cflags & REG_EXTENDED)
	    ? RE_SYNTAX_POSIX_EXTENDED
	    : RE_SYNTAX_POSIX_BASIC);

  if (!(cflags & REG_ICASE))
    preg->translate = 0;
  else
    {
      unsigned i;

      preg->translate = (unsigned char *) malloc (256);
      if (!preg->translate)
        return (int) REG_ESPACE;

      /* Map uppercase characters to corresponding lowercase ones.  */
      for (i = 0; i < CHAR_SET_SIZE; i++)
        preg->translate[i] = isupper (i) ? tolower (i) : i;
    }


  /* If REG_NEWLINE is set, newlines are treated differently.  */
  if (!(cflags & REG_NEWLINE))
    preg->newline_anchor = 0;
  else
    {
      /* REG_NEWLINE implies neither . nor [^...] match newline.  */
      syntax &= ~RE_DOT_NEWLINE;
      syntax |= RE_HAT_LISTS_NOT_NEWLINE;
      /* It also changes the matching behavior.  */
      preg->newline_anchor = 1;
    }

  preg->no_sub = !!(cflags & REG_NOSUB);

  /* POSIX says a null character in the pattern terminates it, so we
   * can use strlen here in compiling the pattern.  
   */

  ret = rx_parse (&preg->pattern,
		  pattern, strlen (pattern),
		  syntax,
		  256,
		  preg->translate);

  /* POSIX doesn't distinguish between an unmatched open-group and an
   * unmatched close-group: both are REG_EPAREN.
   */
  if (ret == REG_ERPAREN)
    ret = REG_EPAREN;

  if (!ret)
    {
      preg->n_subexps = 1;
      preg->subexps = 0;
      rx_posix_analyze_rexp (&preg->subexps,
			     &preg->n_subexps,
			     preg->pattern,
			     0);
      rx_fill_in_fastmap (256,
			  preg->fastmap,
			  preg->pattern);

      preg->is_anchored = rx_is_anchored_p (preg->pattern);
    }

  return (int) ret;
}




/* Returns a message corresponding to an error code, ERRCODE, returned
   from either regcomp or regexec.   */

#ifdef __STDC__
size_t
regerror (int errcode, const regex_t *preg,
	  char *errbuf, size_t errbuf_size)
#else
size_t
regerror (errcode, preg, errbuf, errbuf_size)
    int errcode;
    const regex_t *preg;
    char *errbuf;
    size_t errbuf_size;
#endif
{
  const char *msg;
  size_t msg_size;

  msg = rx_error_msg[errcode] == 0 ? "Success" : rx_error_msg[errcode];
  msg_size = strlen (msg) + 1; /* Includes the 0.  */
  if (errbuf_size != 0)
    {
      if (msg_size > errbuf_size)
        {
          strncpy (errbuf, msg, errbuf_size - 1);
          errbuf[errbuf_size - 1] = 0;
        }
      else
        strcpy (errbuf, msg);
    }
  return msg_size;
}



#ifdef __STDC__
int
rx_regmatch (regmatch_t pmatch[],
	     regex_t *preg,
	     struct rx_context_rules * rules,
	     int start, int end, char *string)
#else
int
rx_regmatch (pmatch, preg, rules, start, end, string)
     regmatch_t pmatch[];
     regex_t *preg;
     struct rx_context_rules * rules;
     int start;
     int end;
     char *string;
#endif
{
  struct rx_solutions * solutions;
  enum rx_answers answer;
  struct rx_context_rules local_rules;
  int orig_end;
  
  local_rules = *rules;
  orig_end = end;

  while (end >= start)
    {
      local_rules.not_eol = (rules->not_eol
			     ? (   (end == orig_end)
				|| !local_rules.newline_anchor
				|| (string[end] != '\n'))
			     : (   (end != orig_end)
				&& (!local_rules.newline_anchor
				    || (string[end] != '\n'))));
      solutions = rx_basic_make_solutions (pmatch, preg->pattern, preg->subexps,
					   start, end, &local_rules, string);
      if (!solutions)
	return REG_ESPACE;
      
      do
	{
	  answer = rx_next_solution (solutions);
	}
      while (answer == rx_maybe);

      rx_basic_free_solutions (solutions);
      if (answer == rx_yes)
	{
	  if (pmatch)
	    {
	      pmatch[0].rm_so = start;
	      pmatch[0].rm_eo = end;
	    }
	  return 0;
	}
      --end;
    }

  switch (answer)
    {
    default:
    case rx_bogus:
      return REG_ESPACE;

    case rx_no:
    case rx_maybe:
      return REG_NOMATCH;
    }
}


#ifdef __STDC__
int
rx_regexec (regmatch_t pmatch[],
	    regex_t *preg,
	    struct rx_context_rules * rules,
	    int start,
	    int end,
	    char *string)
#else
int
rx_regexec (pmatch, preg, rules, start, end, string)
     regmatch_t pmatch[];
     regex_t *preg;
     struct rx_context_rules * rules;
     int start;
     int end;
     char *string;
#endif
{
  int x;
  int stat;
  int anchored;

  anchored = preg->is_anchored;
  for (x = start; x <= end; ++x)
    {
      if (preg->fastmap[((unsigned char *)string)[x]])
	{
	  stat = rx_regmatch (pmatch, preg, rules, x, end, string);
	  if (!stat || (stat != REG_NOMATCH))
	    return stat;
	}
      if (anchored)
	return REG_NOMATCH;
    }
  return REG_NOMATCH;
}



/* regexec searches for a given pattern, specified by PREG, in the
 * string STRING.
 *
 * If NMATCH is zero or REG_NOSUB was set in the cflags argument to
 * `regcomp', we ignore PMATCH.  Otherwise, we assume PMATCH has at
 * least NMATCH elements, and we set them to the offsets of the
 * corresponding matched substrings.
 *
 * EFLAGS specifies `execution flags' which affect matching: if
 * REG_NOTBOL is set, then ^ does not match at the beginning of the
 * string; if REG_NOTEOL is set, then $ does not match at the end.
 *
 * We return 0 if we find a match and REG_NOMATCH if not.  
 */

#ifdef __STDC__
int
regnexec (regex_t *preg, int len, char *string,
	 size_t nmatch, regmatch_t pmatch[],
	 int eflags)
#else
int
regnexec (preg, len, string, nmatch, pmatch, eflags)
     regex_t *preg;
     int len;
     char *string;
     size_t nmatch;
     regmatch_t pmatch[];
     int eflags;
#endif
{
  int want_reg_info;
  struct rx_context_rules rules;
  regmatch_t * regs;
  int nregs;
  int stat;

  want_reg_info = (!preg->no_sub && (nmatch > 0));

  rules.newline_anchor = preg->newline_anchor;
  rules.not_bol = !!(eflags & REG_NOTBOL);
  rules.not_eol = !!(eflags & REG_NOTEOL);

  if (nmatch >= preg->n_subexps)
    {
      regs = pmatch;
      nregs = nmatch;
    }
  else
    {
      regs = (regmatch_t *)malloc (preg->n_subexps * sizeof (*regs));
      nregs = preg->n_subexps;
    }

  {
    int x;
    for (x = 0; x < nregs; ++x)
      regs[x].rm_so = regs[x].rm_eo = -1;
  }


  stat = rx_regexec (pmatch, preg, &rules, 0, len, string);

  if (!stat && want_reg_info && (regs != pmatch))
    {
      int x;
      for (x = 0; x < nmatch; ++x)
	pmatch[x] = regs[x];

      free (regs);
    }

  return ((stat == rx_yes)
	  ? 0
	  : REG_NOMATCH);
}

#ifdef __STDC__
int
regexec (regex_t *preg, char *string,
	 size_t nmatch, regmatch_t pmatch[],
	 int eflags)
#else
int
regexec (preg, string, nmatch, pmatch, eflags)
     regex_t *preg;
     char *string;
     size_t nmatch;
     regmatch_t pmatch[];
     int eflags;
#endif
{
  return regnexec (preg,
		   strlen (string),
		   string,
		   nmatch,
		   pmatch,
		   eflags);
}


/* Free dynamically allocated space used by PREG.  */

#ifdef __STDC__
void
regfree (regex_t *preg)
#else
void
regfree (preg)
    regex_t *preg;
#endif
{
  if (preg->pattern)
    {
      rx_free_rexp (preg->pattern);
      preg->pattern = 0;
    }
  if (preg->subexps)
    {
      free (preg->subexps);
      preg->subexps = 0;
    }
  if (preg->translate != 0)
    {
      free (preg->translate);
      preg->translate = 0;
    }
}

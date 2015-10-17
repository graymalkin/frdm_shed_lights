/*
 * (C) The University of Kent and Simon Cooksey 2015.
 */

#ifndef __debugging_h_
#define __debugging_h_

#define DEBUGGING
#ifdef DEBUGGING
#define DBG(x) x
#else
#define DBG(x)
#endif

#endif // __debugging_h_

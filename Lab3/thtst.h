#ifndef __THTST_H
# define __THTST_H

#ifndef MAX_THREADS
# define MAX_THREADS 20
#endif // MAX_THREADS

# ifndef MAXSHORT
#  define MAXSHORT ((~ 0x0) & (~ (0x1 << (SHORTBITS - 1))))
# endif // MAXSHORT

# ifndef MAXINT
#  define MAXINT ((~ 0x0) & (~ (0x1 << (INTBITS - 1))))
# endif // MAXINT

# ifndef MAXLONG
#  define MAXLONG ((~ 0x0) & (~ (0x1 << (LONGBITS - 1))))
# endif // MAXLONG

# ifndef MINSHORT
#  define MINSHORT ((MAXSHORT * -1) - 1)
# endif // MINSHORT

# ifndef MININT
#  define MININT ((MAXINT * -1) - 1)
# endif // MININT

# ifndef MINLONG
#  define MINLONG ((MAXLONG * -1) - 1)
# endif // MINLONG

#ifndef TRUE
# define TRUE 1
#endif // TRUE
#ifndef FALSE
# define FALSE 0
#endif // FALSE

#endif // __THTST_H

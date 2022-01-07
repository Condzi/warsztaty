#ifndef VARAGS_LENGTH_H
#define VARAGS_LENGTH_H

#include <stdarg.h>

#ifndef VA_ARGS_LENGTH_PRIV
	#define VA_ARGS_LENGTH_PRIV(_1, _2, _3, _4, _5, _6, _7, _8, _N, ...) _N
#endif // VA_ARGS_LENGTH_PRIV

#ifndef VA_ARGS_LENGTH 
	#define VA_ARGS_LENGTH(...) VA_ARGS_LENGTH_PRIV(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1)
#endif // VA_ARGS_LENGTH 

#endif // VARAGS_LENGTH_H
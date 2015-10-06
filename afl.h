#ifndef __AFL_LOOP
#warning "Building without afl-clang-fast(++)"

/* This is a fallback implementation of __AFL_LOOP that returns true once. */
static int __AFL_LOOP(int x)
{
	static int to_do = 1;
	return to_do--;
}

/* Make sure that it's still defined as a macro (just in case) */
#define __AFL_LOOP __AFL_LOOP

#endif

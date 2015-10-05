#ifndef __AFL_LOOP
#warning "Building without afl-clang-fast(++)"
#define __AFL_LOOP(x) 0
#endif

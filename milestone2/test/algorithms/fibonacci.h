#ifndef __FIBONACCI_H__
#define __FIBONACCI_H__

#include <stdint.h>

void *fibonacci_tf_serial(void *n_vp);
intptr_t fibonacci_fast(int n);
int64_t fibonacci_recursive(int64_t n);


#endif
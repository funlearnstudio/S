#ifndef S_NATIVE_TEST_API_H
#define S_NATIVE_TEST_API_H

#include "s/native.h"

#ifdef __cplusplus
extern "C" {
#endif

int s_test_add(int a,int b);
double s_test_square(double x);
int s_test_not(int value);
const char* s_test_echo(const char* text);
int s_test_fail(int value);
SBytesView s_test_bytes_echo(SBytesView bytes);
int s_test_bytes_len(SBytesView bytes);
void* s_test_counter_create(void);
void s_test_counter_destroy(void* raw);
int s_test_counter_value(void* raw);
int s_test_live_counters(void);

#ifdef __cplusplus
}
#endif

#endif

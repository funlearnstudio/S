#include "native_test_api.h"
#include <stdlib.h>
#include <string.h>

int s_test_add(int a,int b){return a+b;}
double s_test_square(double x){return x*x;}
int s_test_not(int value){return value?0:1;}
const char* s_test_echo(const char* text){return text;}
int s_test_fail(int value){(void)value;return -1;}
SBytesView s_test_bytes_echo(SBytesView bytes){return bytes;}
int s_test_bytes_len(SBytesView bytes){return (int)bytes.size;}

static int live_counters=0;
typedef struct STestCounter { int value; } STestCounter;
void* s_test_counter_create(void){STestCounter* c=(STestCounter*)malloc(sizeof(STestCounter));if(!c)return NULL;c->value=42;++live_counters;return c;}
void s_test_counter_destroy(void* raw){if(!raw)return;free(raw);--live_counters;}
int s_test_counter_value(void* raw){return raw?((STestCounter*)raw)->value:-1;}
int s_test_live_counters(void){return live_counters;}

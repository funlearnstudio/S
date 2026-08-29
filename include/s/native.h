#ifndef S_NATIVE_H
#define S_NATIVE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SBytesView {
    const uint8_t* data;
    size_t size;
} SBytesView;

#ifdef __cplusplus
}
#endif

#endif

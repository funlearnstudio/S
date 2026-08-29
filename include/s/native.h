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

/* SE 0.3 public spelling. SBytesView remains as a source-compatible ABI alias. */
typedef SBytesView SEBytesView;

#ifdef __cplusplus
}
#endif

#endif

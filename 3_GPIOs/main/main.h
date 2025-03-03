#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

bool Timeout_Expired(uint64_t start, uint64_t time_us);

#ifdef __cplusplus
}
#endif

#endif // MAIN_H

#ifndef FITS_UTILS_H
#define FITS_UTILS_H
#include "fits/constants.h"


inline size_t needed_padding(size_t size) {
    if (size % BLOCK_SIZE == 0) {
        return 0;
    }
    return BLOCK_SIZE - (size % BLOCK_SIZE);
}

inline size_t add_padding(size_t size) {
    size += needed_padding(size);
    return size;
}

#endif /* ifndef FITS_UTILS_H */

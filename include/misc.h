#ifndef _LIGHTX_MISC_H_
#define _LIGHTX_MISC_H_

static inline int
pad_to_int32(const int bytes)
{
    return (((bytes) + 3) & ~3);
}

#endif











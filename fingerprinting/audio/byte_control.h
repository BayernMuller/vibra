#ifndef BYTE_CONTROL_H
#define BYTE_CONTROL_H

#include <cstdint>

#define GETINTX(T, cp, i)  (*(T *)((unsigned char *)(cp) + (i)))
#define SETINTX(T, cp, i, val)  do {                    \
        *(T *)((unsigned char *)(cp) + (i)) = (T)(val); \
    } while (0)


#define GETINT8(cp, i)          GETINTX(signed char, (cp), (i))
#define GETINT16(cp, i)         GETINTX(int16_t, (cp), (i))
#define GETINT32(cp, i)         GETINTX(int32_t, (cp), (i))

#ifdef WORDS_BIGENDIAN
#define GETINT24(cp, i)  (                              \
        ((unsigned char *)(cp) + (i))[2] +              \
        (((unsigned char *)(cp) + (i))[1] * (1 << 8)) + \
        (((signed char *)(cp) + (i))[0] * (1 << 16)) )
#else
#define GETINT24(cp, i)  (                              \
        ((unsigned char *)(cp) + (i))[0] +              \
        (((unsigned char *)(cp) + (i))[1] * (1 << 8)) + \
        (((signed char *)(cp) + (i))[2] * (1 << 16)) )
#endif


#define SETINT8(cp, i, val)     SETINTX(signed char, (cp), (i), (val))
#define SETINT16(cp, i, val)    SETINTX(int16_t, (cp), (i), (val))
#define SETINT32(cp, i, val)    SETINTX(int32_t, (cp), (i), (val))

#ifdef WORDS_BIGENDIAN
#define SETINT24(cp, i, val)  do {                              \
        ((unsigned char *)(cp) + (i))[2] = (int)(val);          \
        ((unsigned char *)(cp) + (i))[1] = (int)(val) >> 8;     \
        ((signed char *)(cp) + (i))[0] = (int)(val) >> 16;      \
    } while (0)
#else
#define SETINT24(cp, i, val)  do {                              \
        ((unsigned char *)(cp) + (i))[0] = (int)(val);          \
        ((unsigned char *)(cp) + (i))[1] = (int)(val) >> 8;     \
        ((signed char *)(cp) + (i))[2] = (int)(val) >> 16;      \
    } while (0)
#endif


#define GETRAWSAMPLE(size, cp, i)  (                    \
        (size == 1) ? (int)GETINT8((cp), (i)) :         \
        (size == 2) ? (int)GETINT16((cp), (i)) :        \
        (size == 3) ? (int)GETINT24((cp), (i)) :        \
                      (int)GETINT32((cp), (i)))

#define SETRAWSAMPLE(size, cp, i, val)  do {    \
        if (size == 1)                          \
            SETINT8((cp), (i), (val));          \
        else if (size == 2)                     \
            SETINT16((cp), (i), (val));         \
        else if (size == 3)                     \
            SETINT24((cp), (i), (val));         \
        else                                    \
            SETINT32((cp), (i), (val));         \
    } while(0)


#define GETSAMPLE32(size, cp, i)  (                           \
        (size == 1) ? (int)GETINT8((cp), (i)) * (1 << 24) :   \
        (size == 2) ? (int)GETINT16((cp), (i)) * (1 << 16) :  \
        (size == 3) ? (int)GETINT24((cp), (i)) * (1 << 8) :   \
                      (int)GETINT32((cp), (i)))

#define SETSAMPLE32(size, cp, i, val)  do {     \
        if (size == 1)                          \
            SETINT8((cp), (i), (val) >> 24);    \
        else if (size == 2)                     \
            SETINT16((cp), (i), (val) >> 16);   \
        else if (size == 3)                     \
            SETINT24((cp), (i), (val) >> 8);    \
        else                                    \
            SETINT32((cp), (i), (val));         \
    } while(0)

#endif // BYTE_CONTROL_H
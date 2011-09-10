#ifndef UTIL_CPP_H
#define UTIL_CPP_H

#define true 1
#define false 0
#define NULL 0


#define PACKED __attribute__((packed))
#define ALIGN(x) __attribute__ ((aligned (x)))

typedef unsigned int   u32int;
typedef          int   s32int;
typedef unsigned short u16int;
typedef          short s16int;
typedef unsigned char  u8int;
typedef          char  s8int;
typedef u32int         size_t;


extern void initialiseConstructors();


extern void *memset(void *s, char d, int l);
extern void *memcpy(void *dest, const void *src, int n);
extern void outb(u16int port, u8int);
extern u8int inb(u16int port);
extern u16int inw(u16int port);
#endif 
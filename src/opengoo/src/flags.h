#ifndef FLAGS_H
#define FLAGS_H


static const unsigned char STANDARD  = 0;
static const unsigned char DEBUG     = 1<<0;
static const unsigned char OPENGL    = 1<<1;
static const unsigned char ONLYTEXT  = 1<<2;
static const unsigned char FPS       = 1<<3;

extern unsigned char flag;

#endif // FLAGS_H

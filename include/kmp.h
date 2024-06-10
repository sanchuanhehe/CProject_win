#ifndef KMP_H
#define KMP_H

struct STR
{
    int len;
    char *str;
};

typedef struct STR Str;

typedef struct STR *pStr;

void GetNext(Str str, int *next);

int KMP(Str s, Str p, int next[]);

#endif // !KMP_H
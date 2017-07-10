// bv.h — Bit Vector interface
// Author: Prof. Darrell Long

# ifndef _BV_H
# define _BV_H
# include <stdint.h>
# include <stdlib.h>

typedef struct bitV {
        uint8_t *v;
        uint32_t l;
} bitV;

static inline bitV *newVec(uint32_t l)
{
        bitV *v = (bitV *) malloc(sizeof(bitV));
        v->v = (uint8_t *) calloc(l / 8 + 1, sizeof(uint8_t));
        v->l = l;
        return v;
}

static inline void delVec(bitV *v)
{
        free(v->v); 
        free(v);
        return;
}

static inline void oddVec(bitV *v)
{
        for (uint32_t i = 0; i < v->l / 8 + 1; i += 1)
        {
                v->v[i] = 0xaa;
        }
        return;
}

static inline void printVec(bitV *v)
{
        for(int i = 0; i < (int)(v->l)/8; i++)
        {
                printf("%i ", v->v[i]);
        }
        printf("\n");
}

# define setBit(x, k) x->v[k / 8] |=  (0x1 << k % 8)

# define clrBit(x, k) x->v[k / 8] &= ~(0x1 << k % 8)

# define valBit(x, k) ((x->v[k / 8] & (0x1 << k % 8)) != 0)

# define lenVec(x) (x->l)

# endif
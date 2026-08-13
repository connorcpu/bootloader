#include <stdint.h>

#define GET_FLOAT_WORD(i,d)      \
do {                             \
   ieee_float_shape_type gf_u;   \
   gf_u.value = (d);             \
   (i) = gf_u.word;              \
} while (0)

#define SET_FLOAT_WORD(d,i)      \
do {                             \
   ieee_float_shape_type sf_u;   \
   sf_u.word = (i);              \
   (d) = sf_u.value;             \
}while (0)

typedef union
{
   float value;
   uint32_t word;
}ieee_float_shape_type;


uint64_t abs(int n);
float roundf(float n);

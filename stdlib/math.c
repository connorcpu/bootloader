#include "math.h"

uint64_t abs(int n){

   return n < 0 ? -n : n; 

}

float roundf(float x){

  int signbit;
  uint32_t w;
  /* Most significant word, least significant word. */
  int exponent_less_127;

  GET_FLOAT_WORD(w, x);

  /* Extract sign bit. */
  signbit = w & 0x80000000;

  /* Extract exponent field. */
  exponent_less_127 = (int)((w & 0x7f800000) >> 23) - 127;

  if (exponent_less_127 < 23)
    {
      if (exponent_less_127 < 0)
        {
          w &= 0x80000000;
          if (exponent_less_127 == -1)
            /* Result is +1.0 or -1.0. */
            w |= ((uint32_t)127 << 23);
        }
      else
        {
          unsigned int exponent_mask = 0x007fffff >> exponent_less_127;
          if ((w & exponent_mask) == 0)
            /* x has an integral value. */
            return x;

          w += 0x00400000 >> exponent_less_127;
          w &= ~exponent_mask;
        }
    }
  else
    {
      if (exponent_less_127 == 128)
        /* x is NaN or infinite. */
        return x + x;
      else
        return x;
    }
  SET_FLOAT_WORD(x, w);
  return x;

}

enum parse_state {
    INITIAL,
    DECIMAL,
};

typedef enum parse_state parse_state;

float parseFloat(char *text) {
    float x = 0;
    int d = 0;

    char *s = text;
    parse_state state = INITIAL;

    if (*s == '-')
        s++;

    while (*s && *s != ' ' && *s != '\n') {
        switch (state) {
            case INITIAL:
                if (*s >= '0' && *s <= '9') {
                    x *= 10;
                    x += *s - '0';
                } else if (*s == '.') {
                    state = DECIMAL;
                } else {
                    return NAN;
                }

                s++;
                break;
            case DECIMAL:
                if (*s >= '0' && *s <= '9') {
                    x *= 10;
                    x += *s - '0';
                    d += 1;
                } else {
                    return NAN;
                }

                s++;
                break;
        }
    }

    float y = x / ipow(10, d);
    return *text == '-' ? -y : y;
}

int ipow(int base, int exp){

   int result = 1;
   for(;;){
      
      if(exp & 1)result *= base;
      exp >>= 1;
      if(!exp) break;
      base *= base;

   }
   return result;

}


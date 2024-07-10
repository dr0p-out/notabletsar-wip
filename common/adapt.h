#ifndef __COMMON_ADAPT_H__
#define __COMMON_ADAPT_H__

#include <stdint.h>

int
calc_scale (uint16_t w, uint16_t h,
            uint16_t *off_x, uint16_t *off_y,
            double *factor);

#endif  /* __COMMON_ADAPT_H__ */

#include <stdint.h>

#include "adapt.h"
#include "host_info.h"

int
calc_scale (uint16_t w, uint16_t h,
            uint16_t *off_x, uint16_t *off_y,
            double *factor)
{
  uint16_t host_w;
  uint16_t host_h;
  double f;
  uint16_t other_scaled;

  if (host_scrsize (&host_w, &host_h) < 0)
    return -1;

  f = (double)host_w / (double)w;
  other_scaled = (uint16_t)(h * f);
  if (other_scaled <= host_h)
  {
    *off_x = 0;
    *off_y = (host_h - other_scaled) / 2;
  }
  else
  {
    f = (double)host_h / (double)h;
    other_scaled = (uint16_t)(w * f);
    *off_x = (host_w - other_scaled) / 2;
    *off_y = 0;
  }

  *factor = f;

  return 0;
}

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "global.h"

unsigned int history[1024] = {0};
unsigned int history_len = sizeof(history) / sizeof(*history);

void get_heatmap_color(uint8_t input) {
  unsigned short i;
  unsigned int sum = 0;
  unsigned int avg = 0;

  for (i=0; i<history_len; i++) {
      history[history_len] = input; /* last element gets new input */
      history[i] = history[i+1];     /* shift elements left by 1 position */
      sum += history[i];             /* sum of all values stored in array */
  }

  avg = sum/history_len;

  #ifdef DEBUG
      printf("DEBUG: get_heatmap_color(): sum=%d, avg=%d\n", sum, avg);
  #endif

  sum = 0;

       if (avg > 90) { R = 205; G =   0; B =   0; } /* bloodred */
  else if (avg > 80) { R = 240; G =   0; B =   0; } /* red */
  else if (avg > 70) { R = 255; G =  69; B =   0; } /* orangered */
  else if (avg > 60) { R = 255; G =  99; B =  71; } /* tomato */
  else if (avg > 50) { R = 255; G = 165; B =   0; } /* orange */
  else if (avg > 40) { R = 255; G = 244; B =   0; } /* yellow */
  else if (avg > 30) { R = 173; G = 255; B =  47; } /* greenyellow */
  else if (avg > 20) { R =  93; G = 233; B = 233; } /* cyan */
  else if (avg > 10) { R =   0; G = 191; B = 255; } /* deepskyblue */
  else if (avg >= 0) { R =   0; G =   0; B = 211; } /* deepblue */

  #ifdef DEBUG
      printf("DEBUG: get_heatmap_color() returned: RGB=%d, %d, %d avg=%d\n",
              R, G, B, avg);
  #endif
}

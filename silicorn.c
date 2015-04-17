/* Silicorn will lit up a CPU histogram on your Unicorn HAT
 * based on piped input from stdin. */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "debug.h"
#include "global.h"
#include "hat.c"
#include "heatmap.c"

/* Column number that gets fresh input */
#define FRESHCOL  7
/* Rotate display by 0, 90, 180, 270 degrees clockwise */
#define ROTATION 90

uint8_t arr[8],
        x,
        y,
        ceiling,
        int_from_stdin = 0;

int R = 0,
    G = 0,
    B = 0;

int main(void) {
  sig_handler(); /* Clears HAT on ANY exit. */

  if (board_info_init() < 0) {
      return -1;
  }
  if (ws2811_init(&ledstring)) {
      return -1;
  }

  char buf[5] = "0\n";
  /* value from stdin */
  fgets(buf, sizeof buf, stdin);

  while (buf[strlen(buf)-1] == '\n') {

     int_from_stdin = strtol(buf, NULL, 10);

    if (int_from_stdin) {
      arr[FRESHCOL] = int_from_stdin;
    }
    else {
      /* garbage from stdin */
      arr[FRESHCOL] = 0;
    }

    /* Get new color from heatmap based on avg from
     * previous 1024 values. Pass in fresh value. */
    get_heatmap_color(arr[FRESHCOL]);

    for (x=0; x<8; x++) {
      /* normalize input to fit in range 1-8
       * From Dr. Math: http://mathforum.org/library/drmath/view/60433.html
       * from_scale=0-100 (A and B), to_scale=1-8, input=x,
       * normalized_output=y
       *      y = 1 + (x-A)*(8-1)/(B-A)
       */
      ceiling = round(0.8 + ((arr[x]) * 7.0 / 100.0));
      #ifdef DEBUG
        float rawceiling;
        rawceiling = 0.8 + ((arr[x]) * 7.0 / 100.0);
      #endif

      /* If input is 0, don't light up any pixels.
       * If >100, light up whole row (y) */
      if (arr[x] == 0) {
        ceiling = 1;
      }
      if (arr[x] > 100) {
        ceiling = 8;
      }

      for (y=0; y<ceiling; y++) {
        set_pixel_color(get_pixel_position(y, x, ROTATION), &R, &G, &B);
      }

      #ifdef DEBUG
        printf("DEBUG: x=%d, rawceiling=%.2f, ceiling=%d, input=%3d "
               "RGB=%3d %3d %3d\n", x, rawceiling, ceiling, arr[x], R, G, B);
      #endif

      arr[x] = arr[x+1];
    } //for x

    /* render HAT */
    ws2811_render(&ledstring);

    /* clear buffer */
    memcpy(buf, "0\n", sizeof buf);
    /* read into buffer */
    fgets(buf, sizeof buf, stdin);
    #ifdef DEBUG
      printf("\nDEBUG: From stdin, reading into buf=%s", buf);
    #endif

    /* We COULD probably sleep for much less
     * and the HAT still render alright. */
    usleep(70000);
    clear_led_buffer();
  } /* end while */

  unicorn_exit(0);
  return 0;
}

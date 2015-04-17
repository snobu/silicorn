/* Substantial chunk of code "borrowed" from Pimoroni's repo,
 * http://github.com/pimoroni/unicorn-hat
 */

#include "ws2811.h"
#include "board_info.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include "global.h"

/* Unicorn Hat */
#define TARGET_FREQ     WS2811_TARGET_FREQ
#define GPIO_PIN        18
#define DMA             5
#define WIDTH           8
#define HEIGHT          8
#define LED_COUNT       (WIDTH * HEIGHT)
#define BRIGHTNESS      6 /* 0-255 */


/* Init matrix */
ws2811_t ledstring =
{
        .freq = TARGET_FREQ,
        .dmanum = DMA,
        .channel =
        {
                [0] =
                {
                        .gpionum    = GPIO_PIN,
                        .count      = LED_COUNT,
                        .invert     = 0,
                        .brightness = BRIGHTNESS,
                }
        }
};


/* Return pixel id (integer) */
int get_pixel_position(int x, int y, int rotation){
        int pixel;
        int map[8][8] = {
                {7 ,6 ,5 ,4 ,3 ,2 ,1 ,0 },
                {8 ,9 ,10,11,12,13,14,15},
                {23,22,21,20,19,18,17,16},
                {24,25,26,27,28,29,30,31},
                {39,38,37,36,35,34,33,32},
                {40,41,42,43,44,45,46,47},
                {55,54,53,52,51,50,49,48},
                {56,57,58,59,60,61,62,63}
        };

        switch (rotation) {
             case 0:
                 pixel = map[x][y];
                 break;

             case 90:
                 pixel = map[y][7-x];
                 break;

             case 180:
                 pixel = map[7-x][7-y];
                 break;

             case 270:
                 pixel = map[7-y][x];
                 break;

             default:
                 pixel = map[x][y];
                 break;
        }
        return pixel;
}

/* Add pixel to render "queue" */
void set_pixel_color(int pixel, int *R, int *G, int *B){
        ledstring.channel[0].leds[pixel] = (*R << 16) | (*G << 8) | *B;
}

void clear_led_buffer(void){
        int pixel;
        for(pixel=0; pixel<LED_COUNT; pixel++){
                ledstring.channel[0].leds[pixel] = 0;
        }
}


/* Clear the display and exit gracefully */
void unicorn_exit(int status){
        puts("\nCleaning up HAT...");
        clear_led_buffer();
        ws2811_render(&ledstring);
        ws2811_fini(&ledstring);
        exit(status);
}

void sig_handler(void){
  /* Handler for all 64 kill signals.
   * You need to unicorn_exit() before dying,
   * or you'll end up with garbage on your HAT.
   */
  int i;
  for (i = 0; i < 64; i++) {
          struct sigaction sa;
          memset(&sa, 0, sizeof(sa));
          sa.sa_handler = unicorn_exit;
          sigaction(i, &sa, NULL);
  }
  #ifdef DEBUG
    puts("DEBUG: sig_handler(): All 64 SIGs hooked.");
  #endif
}




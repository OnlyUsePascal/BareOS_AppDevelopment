#include "../lib/image.h"
#include "../lib/framebf.h"
#include "../lib/uart0.h"
#include "../lib/game_be.h"
#include "../lib/util_str.h"
#include "../lib/game_fe.h"
#include "../lib/math.h"

static const uint64_t *img_data;
static long w_offset = 0, h_offset = 0, w_img, h_img;

inline bool check_bounds(const long offset, const long max) {
        str_debug_num(abs(offset));
        str_debug_num(max);
        return abs(offset) < max;
}

extern void draw_picture(const uint64_t *img, const uint64_t w, const uint64_t h) {
        if (img != NULL) {
                w_offset = 0;
                h_offset = 0;
                w_img = w;
                h_img = h;
                img_data = img;

                framebf_init(GAME_W, GAME_H, GAME_W, GAME_H);
        }

        str_debug("w_offset: ");
        str_debug_num(w_offset);
        str_debug("h_offset: ");
        str_debug_num(h_offset);

        framebf_drawImg(w_offset, h_offset, w, h, img_data);
}

extern void move_picture(const img_direction d) {
        switch (d) {
                case IMG_UP: {
                        if (h_offset < 0) {
                                h_offset += STEP;
                        } else {
                                return;
                        }

                        break;
                } case IMG_DOWN: {
                        if (abs(h_offset) < ((h_img - GAME_H) / 2)) {
                                h_offset -= STEP;
                        } else {
                                return;
                        }

                        break;
                } case IMG_LEFT: {
                        if (w_offset < 0) {
                                w_offset += STEP;
                        } else {
                                return;
                        }

                        break;
                } case IMG_RIGHT: {
                        if (abs(w_offset) < ((w_img - GAME_W) / 2)) {
                                w_offset -= STEP;
                        } else {
                                return;
                        }

                        break;
                } default: {
                        return;
                }
        }

        draw_picture(NULL, w_img, h_img);
}


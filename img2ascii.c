#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHAR_ASPECT 0.5

static const char RAMP_LONG[] =
    " .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
static const char RAMP_SHORT[] = " .:-=+*#%@";

static void usage(const char *prog)
{
    fprintf(stderr,
            "Usage: %s [options] image\n"
            "  -w N   output width in characters (default 100)\n"
            "  -i     invert brightness\n"
            "  -c     colour output (ANSI 24-bit)\n"
            "  -n     no automatic contrast stretch\n"
            "  -s     short character ramp\n"
            "  -h     this help\n",
            prog);
}

int main(int argc, char **argv)
{
    int out_w = 100;
    int invert = 0, color = 0, stretch = 1, short_ramp = 0;
    const char *path = NULL;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-w") && i + 1 < argc) {
            out_w = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "-i")) {
            invert = 1;
        } else if (!strcmp(argv[i], "-c")) {
            color = 1;
        } else if (!strcmp(argv[i], "-n")) {
            stretch = 0;
        } else if (!strcmp(argv[i], "-s")) {
            short_ramp = 1;
        } else if (!strcmp(argv[i], "-h")) {
            usage(argv[0]);
            return 0;
        } else if (argv[i][0] == '-' && argv[i][1]) {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            usage(argv[0]);
            return 1;
        } else {
            path = argv[i];
        }
    }

    if (!path || out_w < 1) {
        usage(argv[0]);
        return 1;
    }

    int w, h, n;
    unsigned char *img = stbi_load(path, &w, &h, &n, 3);
    if (!img) {
        fprintf(stderr, "Could not load '%s': %s\n", path, stbi_failure_reason());
        return 1;
    }

    int out_h = (int)(h * ((double)out_w / w) * CHAR_ASPECT + 0.5);
    if (out_h < 1) out_h = 1;

    const char *ramp = short_ramp ? RAMP_SHORT : RAMP_LONG;
    int ramp_len = (int)strlen(ramp);

    double *lum = malloc(sizeof(double) * out_w * out_h);
    unsigned char *rgb = malloc(3 * (size_t)out_w * out_h);
    if (!lum || !rgb) {
        fprintf(stderr, "Out of memory\n");
        stbi_image_free(img);
        return 1;
    }

    double lo = 255.0, hi = 0.0;
    for (int cy = 0; cy < out_h; cy++) {
        int y0 = (int)((long long)cy * h / out_h);
        int y1 = (int)((long long)(cy + 1) * h / out_h);
        if (y1 <= y0) y1 = y0 + 1;
        if (y1 > h) y1 = h;

        for (int cx = 0; cx < out_w; cx++) {
            int x0 = (int)((long long)cx * w / out_w);
            int x1 = (int)((long long)(cx + 1) * w / out_w);
            if (x1 <= x0) x1 = x0 + 1;
            if (x1 > w) x1 = w;

            double sr = 0, sg = 0, sb = 0;
            int count = 0;
            for (int y = y0; y < y1; y++) {
                const unsigned char *p = img + 3 * ((size_t)y * w + x0);
                for (int x = x0; x < x1; x++, p += 3) {
                    sr += p[0];
                    sg += p[1];
                    sb += p[2];
                    count++;
                }
            }
            sr /= count; sg /= count; sb /= count;

            
            double l = 0.2126 * sr + 0.7152 * sg + 0.0722 * sb;
            size_t idx = (size_t)cy * out_w + cx;
            lum[idx] = l;
            rgb[3 * idx + 0] = (unsigned char)(sr + 0.5);
            rgb[3 * idx + 1] = (unsigned char)(sg + 0.5);
            rgb[3 * idx + 2] = (unsigned char)(sb + 0.5);
            if (l < lo) lo = l;
            if (l > hi) hi = l;
        }
    }

    double range = hi - lo;
    if (!stretch || range < 1.0) { lo = 0.0; range = 255.0; }

    for (int cy = 0; cy < out_h; cy++) {
        for (int cx = 0; cx < out_w; cx++) {
            size_t idx = (size_t)cy * out_w + cx;
            double t = (lum[idx] - lo) / range;      
            if (t < 0) t = 0;
            if (t > 1) t = 1;
            if (invert) t = 1.0 - t;

            int ci = (int)(t * (ramp_len - 1) + 0.5);
            char ch = ramp[ci];

            if (color)
                printf("\x1b[38;2;%d;%d;%dm%c", rgb[3 * idx], rgb[3 * idx + 1],
                       rgb[3 * idx + 2], ch);
            else
                putchar(ch);
        }
        if (color) fputs("\x1b[0m", stdout);
        putchar('\n');
    }

    free(lum);
    free(rgb);
    stbi_image_free(img);
    return 0;
}

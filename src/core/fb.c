#include "mb.h"
#include "terminal.h"

unsigned int width, height, pitch, isrgb;
unsigned char *fb;

void fb_init()
{
    mbox[0] = 35*4; // Length of message in bytes
    mbox[1] = MBOX_REQUEST;

    mbox[2] = MBOX_TAG_SETPHYWH; // Tag identifier
    mbox[3] = 8; // Value size in bytes
    mbox[4] = 0;
    mbox[5] = 1080; // Value(width)
    mbox[6] = 720; // Value(height)

    mbox[7] = MBOX_TAG_SETVIRTWH;
    mbox[8] = 8;
    mbox[9] = 8;
    mbox[10] = 1920;
    mbox[11] = 1080;

    mbox[12] = MBOX_TAG_SETVIRTOFF;
    mbox[13] = 8;
    mbox[14] = 8;
    mbox[15] = 0; // Value(x)
    mbox[16] = 0; // Value(y)

    mbox[17] = MBOX_TAG_SETDEPTH;
    mbox[18] = 4;
    mbox[19] = 4;
    mbox[20] = 32; // Bits per pixel

    mbox[21] = MBOX_TAG_SETPXLORDR;
    mbox[22] = 4;
    mbox[23] = 4;
    mbox[24] = 1; // RGB

    mbox[25] = MBOX_TAG_GETFB;
    mbox[26] = 8;
    mbox[27] = 8;
    mbox[28] = 4096; // FrameBufferInfo.pointer
    mbox[29] = 0;    // FrameBufferInfo.size

    mbox[30] = MBOX_TAG_GETPITCH;
    mbox[31] = 4;
    mbox[32] = 4;
    mbox[33] = 0; // Bytes per line

    mbox[34] = MBOX_TAG_LAST;

    // Check call is successful and we have a pointer with depth 32
    if (mbox_call(MBOX_CH_PROP) && mbox[20] == 32 && mbox[28] != 0) {
        mbox[28] &= 0x3FFFFFFF; // Convert GPU address to ARM address
        width = mbox[10];       // Actual physical width
        height = mbox[11];      // Actual physical height
        pitch = mbox[33];       // Number of bytes per line
        isrgb = mbox[24];       // Pixel order
        fb = (unsigned char *)((long)mbox[28]);
    }
}

void drawPixel(int x, int y, unsigned char attr)
{
    int offs = (y * pitch) + (x * 4);
    *((unsigned int*)(fb + offs)) = vgapal[attr & 0x0f];
}

void drawRect(int x0, int y0, int w, int h, unsigned char attr, int fill)
{
    int x1 = x0 + w;
    int y1 = y0 + w;
    int y = y0;
    while (y <= y1) {
        int x = x0;
        while (x <= x1) {
            if ((x == x0 || x == x1) || (y == y0 || y == y1)) drawPixel(x, y, attr);
            else if (fill) drawPixel(x, y, (attr & 0xf0) >> 4);
            x++;
        }
        y++;
    }
}

void drawLine(int x0, int y0, int x1, int y1, unsigned char attr)
{
    int dx, dy, p, x, y;

    dx = x1-x0;
    dy = y1-y0;
    x = x0;
    y = y0;
    p = 2*dy-dx;

    while (x<x1) {
        if (p >= 0) {
            drawPixel(x,y,attr);
            y++;
            p = p+2*dy-2*dx;
        } else {
            drawPixel(x,y,attr);
            p = p+2*dy;
        }
        x++;
    }
}

void drawCircle(int x0, int y0, int radius, unsigned char attr, int fill)
{
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        if (fill) {
            drawLine(x0 - y, y0 + x, x0 + y, y0 + x, (attr & 0xf0) >> 4);
            drawLine(x0 - x, y0 + y, x0 + x, y0 + y, (attr & 0xf0) >> 4);
            drawLine(x0 - x, y0 - y, x0 + x, y0 - y, (attr & 0xf0) >> 4);
            drawLine(x0 - y, y0 - x, x0 + y, y0 - x, (attr & 0xf0) >> 4);
        }
    	drawPixel(x0 - y, y0 + x, attr);
    	drawPixel(x0 + y, y0 + x, attr);
    	drawPixel(x0 - x, y0 + y, attr);
        drawPixel(x0 + x, y0 + y, attr);
    	drawPixel(x0 - x, y0 - y, attr);
    	drawPixel(x0 + x, y0 - y, attr);
    	drawPixel(x0 - y, y0 - x, attr);
    	drawPixel(x0 + y, y0 - x, attr);

    	if (err <= 0) {
    	    y += 1;
    	    err += 2*y + 1;
    	}

    	if (err > 0) {
    	    x -= 1;
    	    err -= 2*x + 1;
    	}
    }
}

void drawChar(unsigned char ch, int x, int y, unsigned char attr, int zoom)
{
    unsigned char *glyph = (unsigned char *)&font + (ch < FONT_NUMGLYPHS ? ch : 0) * FONT_BPG;

    for (int i=1;i<=FONT_HEIGHT * zoom;i++) {
    	for (int j=0;j<FONT_WIDTH * zoom;j++) {
    	    unsigned char mask = 1 << (j / zoom);
    	    unsigned char col = (*glyph & mask) ? attr & 0x0f : (attr & 0xf0) >> 4;
    	    drawPixel(x+j, y+i, col);
    	}
        glyph += (i % zoom) ? 0 : FONT_BPL;
    }
}

void drawString(int x0, int y0, char *s, unsigned char attr, int zoom)
{
    int x = x0;
    int y = y0;
    while (*s) {
        if (*s == '\r') {
            x = x0;
        } else if (*s == '\n') {
            x = x0; y += FONT_HEIGHT * zoom;
        } else {
            drawChar(*s, x, y, attr, zoom);
            x += FONT_WIDTH * zoom;
        }
        s++;
    }
}

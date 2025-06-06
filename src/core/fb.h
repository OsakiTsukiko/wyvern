#ifndef CORE_FB_H_
#define CORE_FB_H_

void fb_init();
void drawPixel(int x, int y, unsigned char attr);
void drawChar(unsigned char ch, int x, int y, unsigned char attr, int zoom);
void drawString(int x, int y, const char* s, unsigned char attr, int zoom);
void drawRect(int x, int y, int w, int h, unsigned char attr, int fill);
void drawCircle(int x0, int y0, int radius, unsigned char attr, int fill);
void drawLine(int x0, int y0, int x1, int y1, unsigned char attr);

#endif

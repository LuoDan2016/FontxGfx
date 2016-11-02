#include "FCanvas.h"

FCanvas1::FCanvas1(uint16_t w, uint16_t h) : m_fontx(this),GFXcanvas1(w, h)
{
  wrap = false;
}

FCanvas1::~FCanvas1(void)
{
}

size_t FCanvas1::write(uint8_t c)
{
  return m_fontx.write(c, textsize, wrap, textcolor, textbgcolor, gfxFont);
}

void FCanvas1::getTextBounds(char *string, int16_t x0, int16_t y0,
			    int16_t *x1, int16_t *y1,
			    uint16_t *w, uint16_t *h)
{
  m_fontx.getTextBounds(string,x0,y0,x1,y1,w,h, textsize, wrap);
}

void FCanvas1::dump(void)
{
  uint8_t *p = getBuffer();
  uint16_t w = width();
  uint16_t h = height();
  int x,y;

  for(y=0;y<h;y++){
    for(x=0;x<(w+7)/8;x++)
      Serial.printf(" %02x", *p++);
    Serial.println();
  }
}

void FCanvas1::fillRect(int16_t x,int16_t y,int16_t w,int16_t h,uint16_t color)
{
  GFXcanvas1::fillRect(x,y,w,h,color);
}

void FCanvas1::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  // Serial.printf("%s:%d x:%d y:%d c:%04x\n",__FUNCTION__,__LINE__,x,y,color);
  GFXcanvas1::drawPixel(x,y,color);
}

void FCanvas1::fillScreen(uint16_t color)
{
  GFXcanvas1::fillScreen(color);
}

#if 0
void FCanvas1::setCursor(int16_t x, int16_t y)
{
  GFXcanvas1::setCursor(x,y);
}
#endif

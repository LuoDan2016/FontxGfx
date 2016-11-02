#ifndef _fontx_gfx_h_
#define _fontx_gfx_h_

#include <Arduino.h>
#include "Fontx.h"
#include "Utf8Decoder.h"
#include <Adafruit_GFX.h>

class FontxGfx : public Fontx {
 protected:
  Adafruit_GFX *m_pGfx;
  Utf8Decoder m_u8d;
  
 public:
  FontxGfx(Adafruit_GFX *pGfx);
  size_t write(uint8_t c,uint8_t textsize,bool wrap, uint16_t fg,uint16_t bg,
	       GFXfont *gfxFont);
  void getTextBounds(char *string, int16_t x0, int16_t y0,
		     int16_t *x1, int16_t *y1,
		     uint16_t *w, uint16_t *h,
		     uint8_t textsize, bool wrap);
  
 protected:
  void process_utf8_byte(uint8_t c, int16_t *pX, int16_t *pY,
			 uint8_t textsize, bool wrap, uint16_t fg, uint16_t bg,
			 bool bDraw = true, int16_t *pX2 = NULL);
  void drawFontxGlyph(const uint8_t *glyph,uint8_t w,uint8_t h,
		      uint16_t cx, uint16_t cy,
		      uint8_t textsize, boolean wrap,
		      uint16_t textcolor, uint16_t textbgcolor);
  void virtual lineFeedHook(int16_t *px, int16_t *py, int16_t h) {}
};



#endif /* _fontx_gfx_h_ */

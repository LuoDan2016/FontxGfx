#ifndef _fcanvas_h_
#define _fcanvas_h_

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include "Utf8Decoder.h"
#include "FontxGfx.h"

class FCanvas1 : public GFXcanvas1 {
 protected:
  FontxGfx m_fontx;
  Utf8Decoder m_u8d;
  
 public:
  FCanvas1(uint16_t w, uint16_t h);
  ~FCanvas1(void);
  size_t write(uint8_t) override;
  void resetFontx(void) { m_fontx.resetFontx();}
  void setFontx(uint8_t *f0, uint8_t *f1=NULL, uint8_t *f2=NULL){
    m_fontx.setFontx(f0, f1, f2);
  }
  void getTextBounds(char *string, int16_t x0, int16_t y0,
		     int16_t *x1, int16_t *y1,
		     uint16_t *w, uint16_t *h);
  void dump(void);
  void fillRect(int16_t x,int16_t y,int16_t w,int16_t h,uint16_t color) override;
  void fillScreen(uint16_t color) override;
  void drawPixel(int16_t x, int16_t y, uint16_t color) override;
};

#endif /* _fcanvas_h_ */

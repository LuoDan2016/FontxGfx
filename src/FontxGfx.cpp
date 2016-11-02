#ifdef __AVR__
 #include <avr/pgmspace.h>
#elif defined(ESP8266)
 #include <pgmspace.h>
#endif
#include "FontxGfx.h"

FontxGfx::FontxGfx(Adafruit_GFX *pGfx) : Fontx()
{
  m_pGfx = pGfx;
}

size_t FontxGfx::write(uint8_t c, uint8_t textsize, bool wrap,
		       uint16_t fg, uint16_t bg, GFXfont *gfxFont)
{
  int16_t cursor_x = m_pGfx->getCursorX();
  int16_t cursor_y = m_pGfx->getCursorY();
  uint16_t _width = m_pGfx->width();
  
  if(m_cFontx == 0) {
    if(!gfxFont) { // 'Classic' built-in font
      if(c == '\n') {
	cursor_y += textsize*(m_spaceY + 8);
	cursor_x  = 0;
	lineFeedHook(&cursor_x, &cursor_y, textsize*(m_spaceY+8));
      } else if(c == '\r') {
	// skip em
      } else {
	if(wrap && ((cursor_x + textsize * 6) >= _width)) { // Heading off edge?
	  cursor_x  = 0;            // Reset x to zero
	  cursor_y += textsize * 8; // Advance y one line
	  lineFeedHook(&cursor_x, &cursor_y, textsize*8);
	}
	m_pGfx->drawChar(cursor_x, cursor_y, c, fg, bg, textsize);
	cursor_x += textsize * 6;
      }
    } else { // Custom font
      if(c == '\n') {
	cursor_x  = 0;
	int16_t lh = (int16_t)textsize *
	  ((uint8_t)pgm_read_byte(&gfxFont->yAdvance) + m_spaceY); 
	cursor_y += lh;
	lineFeedHook(&cursor_x, &cursor_y, lh);
      } else if(c != '\r') {
	uint8_t first = pgm_read_byte(&gfxFont->first);
	if((c >= first) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last))) {
	  uint8_t   c2    = c - pgm_read_byte(&gfxFont->first);
	  GFXglyph *glyph =
	    &(((GFXglyph *)pgm_read_dword(&gfxFont->glyph))[c2]);
	  uint8_t w = pgm_read_byte(&glyph->width);
	  uint8_t h = pgm_read_byte(&glyph->height);
	  if((w > 0) && (h > 0)) { // Is there an associated bitmap?
	    int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset); // sic
	    if(wrap && ((cursor_x + textsize * (xo + w)) >= _width)) {
	      // Drawing character would go off right edge; wrap to new line
	      cursor_x  = 0;
	      cursor_y += (int16_t)textsize *
		(uint8_t)pgm_read_byte(&gfxFont->yAdvance);
	    }
	    m_pGfx->drawChar(cursor_x, cursor_y, c, fg, bg, textsize);
	  }
	  cursor_x += pgm_read_byte(&glyph->xAdvance) * (int16_t)textsize;
	}
      }
    }
    
  } else {
    process_utf8_byte(c, &cursor_x, &cursor_y, textsize, wrap, fg, bg);
  }
  m_pGfx->setCursor(cursor_x, cursor_y);
  return 1;
}

    
void FontxGfx::process_utf8_byte(uint8_t c, int16_t *pX, int16_t *pY,
				 uint8_t textsize, bool wrap,
				 uint16_t fg, uint16_t bg,
				 bool bDraw, int16_t *pX2)
{
  uint32_t ucode;
  const uint8_t *glyph;
  uint8_t w,h;
  uint32_t sjis;
  uint8_t s = textsize < 1 ? 0 : textsize;
  
  if(pX2) *pX2 = *pX;
  
  if(c == '\n'){
    uint8_t h;
    if(getGlyph(' ', NULL, NULL, &h))
      *pY += (h + m_spaceY) * s;
    *pX = 0;
    lineFeedHook(pX, pY, (h + m_spaceY) * s);
  }
  else {
    if(m_u8d.decode(c, &ucode) && getGlyph(ucode, &glyph, &w, &h)){
      if(wrap && (*pX + (w + m_spaceX) * s > m_pGfx->width())){
	*pX = 0;
	if(pX2) *pX2 = 0;
	*pY += (h + m_spaceY) * s;
	lineFeedHook(pX, pY, (h + m_spaceY * s));
      }
      if(bDraw)
	drawFontxGlyph(glyph, w, h, *pX, *pY, textsize, wrap, fg, bg);
      *pX += (w + m_spaceX) * textsize;
    }
  }
}


void
FontxGfx::drawFontxGlyph(const uint8_t *glyph,uint8_t w,uint8_t h,
			 uint16_t cx, uint16_t cy,
			 uint8_t textsize, boolean wrap,
			 uint16_t textcolor, uint16_t textbgcolor)
{
  const uint8_t *gp;
  uint8_t x,y;
  uint16_t xp, yp;
  uint8_t s = textsize < 1 ? 0 : textsize;

  if(glyph == NULL) return;
  
  gp = glyph;
  yp = cy;
  for (y = 0; y < h; y++) {
    xp = cx;
    for (x = 0; x < w; x += 8) {
      uint8_t d = pgm_read_byte(&gp[x / 8]);
      for (int i = 0, m = 0x80; i < 8 && i + x < w; i++, m >>= 1) {
	if (d & m)
	  m_pGfx->fillRect(xp, yp, s, s , textcolor);
	else if(textcolor != textbgcolor)
	  m_pGfx->fillRect(xp, yp, s, s , textbgcolor);
	xp += s;
      }
    }
    gp += (w + 7) / 8;
    yp += s;
  }
}


void FontxGfx::getTextBounds(char *string, int16_t x0, int16_t y0,
			     int16_t *x1, int16_t *y1,
			     uint16_t *w, uint16_t *h,
			     uint8_t textsize, bool wrap)
{
  if(!m_cFontx)
    m_pGfx->getTextBounds(string, x0, y0, x1, y1, w, h);
  else {
    char *p = string;
    int16_t x,y,max_x,max_y,min_x,min_y,x2;

    x = min_x = max_x = x2 = x0;
    y = min_y = max_y = y0;
    while(*p){
      process_utf8_byte(*p++, &x, &y, textsize, wrap, 0, 0, false, &x2);

      if(x < min_x) min_x = x;
      if(x > max_x) max_x = x;
      if(x2 < min_x) min_x = x2;
      if(x2 > max_x) max_x = x2;
      if(y < min_y) min_y = y;
      if(y > max_y) max_y = y;
    }

    uint8_t th;
    if(getGlyph(' ', NULL, NULL, &th))
      max_y += th * textsize;

    *x1 = min_x;
    *y1 = min_y;
    *w = max_x > min_x ? max_x - min_x - 1 : 0;
    *h = max_y > min_y ? max_y - min_y - 1 : 0;
  }
}

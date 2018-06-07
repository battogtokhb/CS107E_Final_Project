#include "gl.h"
#include "fb.h"
#include "printf.h"
#include "font.h"
#include "strings.h"

#define FB_DEPTH 4
#define FONT_WIDTH 14
#define FONT_HEIGHT 16


void gl_init(unsigned int width, unsigned int height, unsigned int mode)
{
    fb_init(width, height, FB_DEPTH, mode);
}

void gl_swap_buffer(void)
{

    return fb_swap_buffer();
}

unsigned int gl_get_width(void)
{
    return fb_get_width();
}

unsigned int gl_get_height(void)
{
    return fb_get_height();
}

color_t gl_color(unsigned char r, unsigned char g, unsigned char b)
{
    color_t color = 0xff << 24;
    color |= r << 16;
    color |= g << 8;
    color |= b;
    return color;
}

void gl_clear(color_t c)
{
  unsigned (*im)[fb_get_pitch() / sizeof(unsigned int)] = (unsigned (*)[fb_get_pitch() / sizeof(unsigned int)])fb_get_draw_buffer();
  for (int x = 0; x < fb_get_width(); x++){
    for (int y = 0; y < fb_get_height(); y++){
      if (im[y][x] !=c){
        im[y][x] = c;
      }
    }
  }
}

void gl_draw_pixel(int x, int y, color_t c)
{
     unsigned (*im)[fb_get_pitch() / sizeof(unsigned int)] = (unsigned (*)[fb_get_pitch() / sizeof(unsigned int)])fb_get_draw_buffer();
     if (x < 0 || y < 0){
       ; //do nothing; don't draw pixel
     }
     if (x < fb_get_width() && y < fb_get_height()){
       im[y][x] = c;
     }

}

color_t gl_read_pixel(int x, int y)
{
   unsigned (*im)[fb_get_pitch() / sizeof(unsigned int)] = (unsigned (*)[fb_get_pitch() / sizeof(unsigned int)])fb_get_draw_buffer();
   if (x < 0 || y < 0){
     return 0;
   }
   if (x < fb_get_width() && y < fb_get_height()){
     return im[y][x] ;
   }
   else{
     return 0;
   }

}

void gl_draw_rect(int x, int y, int w, int h, color_t c)
{
    for (int i = x; i < x + w; i++){
      for (int j = y; j < y+h; j++){
        gl_draw_pixel(i, j, c);
      }
    }
}

void gl_draw_char(int x, int y, int ch, color_t c)
{
    int bufsize = gl_get_char_height() * gl_get_char_width();
    unsigned char  buf[bufsize];
    int n = 0;
    if (font_get_char(ch, buf, bufsize ) ){

       for (int i = 0; i < bufsize; i++){
         int draw_x = (i% (gl_get_char_width()) )  + x;
         int draw_y  = y+(i / gl_get_char_width());
         if (buf[i] == 0xFF){

           gl_draw_pixel(draw_x, draw_y, c);
         }
         if (ch == ' '){
           gl_draw_pixel(draw_x, draw_y, GL_BLACK); 
         }
      }
    }
}

void gl_draw_string(int x, int y, char* str, color_t c)
{
    int len = strlen(str);
    for (int i = 0; i < len; i++){
      gl_draw_char(x, y, str[i], c);
      x += gl_get_char_width();
    }
}

unsigned int gl_get_char_height(void)
{
    return font_get_height();
}

unsigned int gl_get_char_width(void)
{
    return font_get_width();
}

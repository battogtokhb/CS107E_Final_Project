#include "mailbox.h"
#include "fb.h"
#include "printf.h"

// This prevents the GPU and CPU from caching mailbox messages
#define GPU_NOCACHE 0x40000000

typedef struct {
  unsigned int width;       // width of the display
  unsigned int height;      // height of the display
  unsigned int virtual_width;  // width of the virtual framebuffer
  unsigned int virtual_height; // height of the virtual framebuffer
  unsigned int pitch;       // number of bytes per row
  unsigned int depth;       // number of bits per pixel
  unsigned int x_offset;    // x of the upper left corner of the virtual fb
  unsigned int y_offset;    // y of the upper left corner of the virtual fb
  unsigned int framebuffer; // pointer to the start of the framebuffer
  unsigned int size;        // number of bytes in the framebuffer
} fb_config_t;

// fb is volatile because the GPU will write to it
static volatile fb_config_t fb __attribute__ ((aligned(16)));


static void fb_print_info(fb_config_t fb){
  printf("\n-----FB INFO-----\n");
 printf("width = %d\n", fb.width );
  printf("height = %d\n", fb.height );
  printf("virtual width = %d\n", fb.virtual_width );
  printf("virtual height = %d\n", fb.virtual_height );
  printf("depth = %d\n", fb.depth );
  printf("size = %d\n", fb.size );
  printf("pitch = %d\n", fb.pitch );
  printf("buffer = %08x\n", fb.framebuffer );
  printf("y_offset = %d\n", fb.y_offset);

  printf("-----FB INFO-----\n\n");
}

void fb_init(unsigned int width, unsigned int height, unsigned int depth, unsigned int mode)
{
  if (mode == FB_DOUBLEBUFFER){
      fb.virtual_height = 2*height;

  }
  else{
      fb.virtual_height = height;
  }

  fb.width = width;
  fb.virtual_width = width;
  fb.height = height;
  fb.depth = depth * 8; // convert number of bytes to number of bits
  fb.x_offset = 0;
  fb.y_offset = 0;

  // the manual requires we to set these value to 0
  // the GPU will return new values
  fb.pitch = 0;
  fb.framebuffer = 0;
  fb.size = 0;

  mailbox_write(MAILBOX_FRAMEBUFFER, (unsigned)&fb + GPU_NOCACHE);
  (void) mailbox_read(MAILBOX_FRAMEBUFFER);

//  fb_print_info(fb);
}


void fb_swap_buffer(void)
{

  if (fb.height*2 == fb.virtual_height){
      if (fb.y_offset == 0){
        fb.y_offset = fb.height;
      }
      else{
        fb.y_offset = 0;
      }
      mailbox_write(MAILBOX_FRAMEBUFFER, (unsigned)&fb + GPU_NOCACHE);
      (void) mailbox_read(MAILBOX_FRAMEBUFFER);

  }
}

unsigned char* fb_get_draw_buffer(void)
{
  if (fb.height*2 == fb.virtual_height){
    unsigned int addr ;
    if (fb.y_offset > 0){
      addr = fb.framebuffer;
    }
    else{
      addr = fb.framebuffer + fb.height* fb.pitch;
    }
    unsigned char (*p)[fb.height * fb.pitch] = (unsigned char (*)[fb.height * fb.pitch]) addr;
    return p[0];
  }
  else{
    unsigned char (*p)[fb.height * fb.pitch] = (unsigned char (*)[fb.height * fb.pitch]) fb.framebuffer;
    return p[0];
  }
}

unsigned int fb_get_width(void)
{
    return fb.width;
}

unsigned int fb_get_height(void)
{
    return fb.height;
}

unsigned int fb_get_depth(void)
{
    return fb.depth / 8;
}

unsigned int fb_get_pitch(void)
{
    return fb.pitch;
}

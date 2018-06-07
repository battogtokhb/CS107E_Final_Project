#include "console.h"
#include "printf.h"
#include "strings.h"
#include "gl.h"
#include "fb.h"
#include "interrupts.h"
#include "keyboard.h"
#include "malloc.h"
#define MAX_OUTPUT_LEN 1024


unsigned char * DRAW_BUFFER;
unsigned char * READ_BUFFER;

static char *text;
/*Struct to keep track of cursor*/
typedef struct {
   int x;
  int y;
} console_t;

static volatile console_t cursor = {0,0};
static volatile int n_cols = 0;
static volatile int n_rows = 0;
static int SPACING= 22;

static int ROWS;
static int COLS;


void console_init(unsigned int nrows, unsigned int ncols)
{
    gl_init(ncols* gl_get_char_width(), nrows*SPACING, GL_DOUBLEBUFFER);

    DRAW_BUFFER = fb_get_draw_buffer();
    gl_swap_buffer();
    READ_BUFFER = fb_get_draw_buffer();
    gl_swap_buffer();
    text = malloc(nrows * ncols);
    memset(text, '\0', nrows * ncols);
    ROWS = nrows;
    COLS = ncols;


}

void console_clear(void)
{
    gl_clear(GL_BLACK);
    cursor.x = 0;
    cursor.y= 0;
}


/*Draw character on screen, keeping track of cursor*/
static void console_draw_char( char c, int add_to_text ){


  if (c == '\n'){
    cursor.y += SPACING;
    cursor.x = 0;
    if (add_to_text){
     text[n_cols * n_rows] = '\0';
     n_cols++;

     n_rows = 0;
    }
  }
  else if (c == '\b'){

    cursor.x -= gl_get_char_width();
    gl_draw_char(cursor.x, cursor.y,  ' ', GL_BLACK );

  if (cursor.x <= 0 ){
    if (cursor.y >= SPACING ){
      cursor.y -= SPACING;
    }
    cursor.x =  gl_get_width();
  }
  if (add_to_text){
    text[n_cols * n_rows] = '\0';
    n_rows--;
    if (n_rows <= 0){
      n_cols--;
      n_rows=0;
    }
  }

}
  else if (c == '\f'){
    console_clear();
    if (add_to_text){

      n_cols = 0;
      n_rows = 0;
    }
  }
  else if (c == '\r'){
    cursor.x = 0;
  }
  else{
    if (cursor.x >= gl_get_width()){
      cursor.x = 0;
      cursor.y += SPACING  ;
      if (add_to_text){
          n_cols++;
          n_rows = 0;


      }
    }

    gl_draw_char(cursor.x, cursor.y,  c, GL_GREEN );
     cursor.x += gl_get_char_width();



  }
}


/*Scroll the text upwards*/
static void console_shift_up()
{


     console_clear();

    int start = n_cols - (ROWS-1) ;
     start = (start < 0) ? -start :start;

     for (int x = start; x < n_cols; x++){
       for (int y = 0; y < strlen( (char *)text[x]) ; y++){
           printf("%c", text[x * y]);
           console_draw_char( text[x *y], 0);
       }
     }

}

/*Draw character onto screen. Scroll entire screen up if necessary*/
static void calculate_cursor(char c){
  if (cursor.y + gl_get_char_height()  > gl_get_height() ){

    console_shift_up();
    console_draw_char(c,  1);

  }
  else{
      console_draw_char(c, 1);

  }
}


/*Function to print character into console*/
int console_printf(const char *format, ...)

{


      if (fb_get_draw_buffer() != DRAW_BUFFER){
       gl_swap_buffer();
     }

       //memset((void *) text, 0, (text[0][0]) * (gl_get_width() / gl_get_char_width()) * (gl_get_height() /gl_get_char_height()) );
       static x = 0;
       static y = 0;
       char  buf[MAX_OUTPUT_LEN];
        va_list args;
        va_start(args, format);
        int a = vsnprintf(buf, MAX_OUTPUT_LEN, format, args);
        //printf("whats in this buf? %s\n", buf);
        for (int i = 0; i < a; i++){

             calculate_cursor( buf[i] );

        }

    //  console_add_to_text(text, buf[0]);
    //  calculate_cursor(text, buf[0]);
  //
  //
  //   if (fb_get_draw_buffer() != DRAW_BUFFER){
  //     gl_swap_buffer();
  //   }
  //     //gl_draw_char(30,30,'a', GL_BLUE);
  //
  //   char  buf[MAX_OUTPUT_LEN];
  //   va_list args;
  //   va_start(args, format);
  //   int a = vsnprintf(buf, MAX_OUTPUT_LEN, format, args);
  //   int len = strlen(buf);
  //     printf("what is buf: %s\n", buf);
  // //  printf("what is len: %d\n", a);
  // //  console_add_to_text( text, 'j');
  // //    calculate_cursor( text, 'j');
  //   for (int i = 0; i < a; i++){
  //     console_add_to_text( text, buf[i]);
  //     calculate_cursor(text, buf[i] );
  //
  //   }
     va_end(args);
     gl_swap_buffer();
	   return 0;
}

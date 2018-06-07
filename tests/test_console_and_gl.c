#include "assert.h"
#include "timer.h"
#include "fb.h"
#include "gl.h"
#include "console.h"
#include "printf.h"
#include "font.h"

/* Note that to use the console, one should only have to
 * call console_init. To use the graphics library, one
 * should only have to call gl_init. If your main() requires
 * more than this, then your code will not pass tests and
 * will likely have many points deducted. Our GL tests
 * will call gl_init then invoke operations; our console
 * tests will call console_init then invoke operations.
 * To guarantee that tests will pass, make sure to also
 * run tests for each component separately.
 */



#define _WIDTH 640
#define _HEIGHT 512

#define _NROWS 10
#define _NCOLS 20


/*Test for multi-line wrapping and scrolling.
Note: these lines are lyrics from Tame Impala's "The Less I Know the Better"*/
static void test_console_multi(void){
  console_init(_NROWS, _NCOLS);

   console_printf(" ** Someone said they live together I ran out the door to get her");
  timer_delay(3);

  console_printf( " ** She was holding hands with Trevor Not the greatest feeling ever");
  timer_delay(3);

    console_printf(" ** Said pull yourself together You should try your luck with Heather");

    timer_delay(3);
    console_printf(" ** And I hope they slept together Oh the less I know the better");

    timer_delay(3);

}

/*Test for new lines*/
static void test_console_newline(void){
    console_init(_NROWS, _NCOLS);
    console_printf("HELLO\n");
    console_printf("WOAH\n");
     console_printf("HI\n");
      console_printf(" ** Someone said they live together I ran out the door to get her\n");
    console_printf( " ** She was holding hands with Trevor Not the greatest feeling ever\n");
      console_printf(" ** Said pull yourself together You should try your luck with Heather\n");

}


/*Test for backspace*/
static void test_console_backspace(void){
    console_init(_NROWS, _NCOLS);
    console_printf("HELLO\b");
    console_printf("HI\n");

      console_printf(" ** Someone said they live together I ran out the door to get her");
    console_printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
    console_printf(" TESTING");

}

/*Test for console_clear() */
static void test_console_clear(void){
    console_init(_NROWS, _NCOLS);
    console_printf("HELLO");
    console_printf("\f");
    console_printf("HI\n");
    console_printf(" ** Someone said they live together I ran out the door to get her\n");
   console_printf( " ** She was holding hands with Trevor Not the greatest feeling ever\n");
   timer_delay(3);
}



static void test_console(void)
{

    console_init(_NROWS, _NCOLS);

    console_printf("HELLO\r\n");



     timer_delay(3);
     console_printf("\f");
     console_printf("HI\r\n");
}

static void test_gl(void)
{
    fb_init(_WIDTH, _HEIGHT, 4, GL_DOUBLEBUFFER);

    gl_clear(gl_color(0xFF, 0, 0xFF)); // Background should be purple.

    // Draw an amber pixel at an arbitrary spot.
    gl_draw_pixel(_WIDTH/3, _HEIGHT/3, GL_AMBER);
    assert(gl_read_pixel(_WIDTH/3, _HEIGHT/3) == GL_AMBER);

    // Basic rectangle should be blue in center of screen
    gl_draw_rect(_WIDTH/2 - 20, _HEIGHT/2 - 20, 40, 40, GL_BLUE);

    gl_draw_rect(_WIDTH/2 - 20, _HEIGHT-30, 60, 100, GL_BLUE); //test partially out of bounds rect

    gl_draw_char(60, 10, 'A', GL_BLUE);
    // Should write a single character
    gl_draw_char(60, -5, 'A', GL_BLUE); //test partially out of bounds char

    gl_draw_string(100, 50, "HELLO WORLD", GL_BLUE);

    gl_draw_string(100, 100, "JOSH IS REALLY SMELLY", GL_BLUE);



    gl_swap_buffer();
}

/*Test for single buffer*/
static void test_single_buf(void){
  //single buffer (code from lecture)
  gl_init(1280, 1024, GL_SINGLEBUFFER);

  assert(fb_get_width() == 1280);
  assert(fb_get_height() == 1024);
  assert(fb_get_depth() == 4); //default value of 4 for depth
  assert(fb_get_pitch() == 1280 * 4);

  while (1) {
      gl_clear(GL_BLACK);
      gl_draw_string(20, 20, "hello, world", GL_WHITE);
  }

}

/*Test for double buffer*/
static void test_double_buf(void){
    //double buffer (code from lecture)
   gl_init(1280, 1024, GL_DOUBLEBUFFER);
    while (1) {
      gl_clear(GL_BLACK);
      gl_draw_string(20, 20, "hello, world 0", GL_WHITE);
      gl_swap_buffer();

      gl_clear(GL_BLACK);
      gl_draw_string(20, 20, "hello, world 1", GL_WHITE);
      gl_swap_buffer();
    }
}

void main(void)
{
    // test_console();
    // test_gl();
    // test_double_buf();
    // test_single_buf();
    // test_console_multi();
    // test_console_newline();
    // test_console_backspace();
     test_console_clear();


}

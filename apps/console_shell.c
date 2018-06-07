#include "gpio.h"
#include "keyboard.h"
#include "console.h"
#include "gl.h"
#include "shell.h"
#include "printf.h"
#include "interrupts.h"

// Main function for assignment 6 (console)
// You should not need to modify this.
// It should boot right into the graphical version of your shell
// from assignment 5.

void main(void)
{
    gpio_init();
    console_init(20, 40);
    //printf("width: %d | height: %d \n", gl_get_width(), gl_get_height());
    keyboard_init();


    shell_init(console_printf);
    

    shell_run();

}

#include "shell.h"
#include "shell_commands.h"
#include "uart.h"
#include "strings.h"
#include "keyboard.h"
#include "malloc.h"
#include "printf.h"
#include "console.h"
#include "pi.h"
#include <stdint.h>

#define LINE_LEN 80
#define COMMANDS_NUM 5

static int (*shell_printf)(const char * format, ...);

static int isdigit(char c)
{
    if (c >= '0' && c <= '9') return 1;
    return 0;
}


//helper function for strtonum
static int isalpha(char c)
{
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) return 1;
    return 0;
}



/*
 * The function strndup() returns a pointer to a new null-terminated string containing at most `n`
 * bytes copied from the string pointed to by `src`.
 *
 */
 static char *strndup(const char *src, int n)
 {
    int len = n;
     char * ptr = malloc(n+1);
     char * saved = ptr;
     while (n > 0){
       *ptr++ = *src++;
       n--;

     }
     ptr[len] = '\0';
     return saved ;
 }


/*
 * The function isspace returns 1 if the char parameter
 * is a space, tab, or new line, and 0 if otherwise
 */
static int isspace(char ch)
{
    return ch == '\0' || ch == '\t' || ch == '\n' || ch == ' ';
}

/*
 * The function tokenize stores sequences of non-space
 * characters (aka tokens) in an array of char *
 */
static int tokenize(const char *line, char *array[],  int max)
{

    int ntokens = 0;

    while (*line != '\0' && ntokens < max) {
        while (isspace(*line)) line++;  // skip past spaces
        if (*line == '\0') break; // no more non-white chars
        const char *start = line;
        while (!isspace(*line)) line++; // advance to next space/null
        int nchars = line - start;
        array[ntokens++] = strndup(start, nchars);   // make heap-copy, add to array
    }
    return ntokens;
}

/*
 * The array commands[] stores valid shell comands represented by
 * type command_t which stores the name, description, and
 * implementation of the command.
 */
static const command_t commands[] = {
    {"help",   "help prints a list of commands or description of cmd", cmd_help},
    {"echo",   "echo echos the user input to the screen", cmd_echo},
    {"reboot", "reboot the Raspberry Pi back to the bootloader", cmd_reboot},
    {"peek", "peek prints the contents (4 bytes) of memory at address", cmd_peek},
    {"poke", "poke stores `value` into the memory at `address`", cmd_poke},
};


/*
 * See shell_commands.h for details on cmd_echo
 */
int cmd_echo(int argc, const char *argv[])
{
    for (int i = 1; i < argc; ++i){
        shell_printf("%s ", argv[i]);
      }
    shell_printf("\n");
    return 0;
}


/*
 * The function isacommand checks if a char * instance
 * is a valid command, if so, it returns the index of that command
 * in the static array commands[]. If not a valid command, return -1.
 */
int isacommand( const char * command){
  if (strcmp(command, "help") == 0) return 0;
  if (strcmp(command, "echo") == 0) return 1;
  if (strcmp(command, "reboot") == 0) return 2;
  if (strcmp(command, "peek") == 0) return 3;
  if (strcmp(command, "poke") == 0) return 4;
  return -1;
}

/*
 * See shell_commands.h for details on cmd_help
 */
int cmd_help(int argc, const char *argv[])
{

    if (argc == 1){
      for (int i = 0; i < COMMANDS_NUM; i++){
        shell_printf("%s: %s\n", commands[i].name, commands[i].description);
      }

    }

    else{
     for (int i = 1; i < argc; ++i){
          int command_ref = isacommand(argv[i]);
          if (command_ref == -1){
              shell_printf("error: no such command '%s'\n", argv[i]);
              return 1;
          }
          else{
            shell_printf("%s: %s\n", commands[command_ref].name, commands[command_ref].description);
          }
         }
     }
    return 0;

}

/*
 * See shell_commands.h for details on cmd_reboot
 */
int cmd_reboot(int argc, const char* argv[]){
  pi_reboot();
  return 0;
}

/*
 * See shell_commands.h for details on cmd_peek
 */
// int cmd_peek(int argc, const char* argv[]){
//   if (argc < 2 || (!isdigit(argv[1][0]) && !isalpha(argv[1][0] )) ){
//     shell_printf("error: peek expects 1 argument [address]\n");
//     return 1;
//   }
//   else{
//     const char * end ;
//     const char * str = argv[1];
//   //   printf("what is str? : %d\n", str);
//     // printf("what is str len? : %d\n", strlen(str));
//     int num = strtonum(str, &end);
//
//     //printf("what is num? : %d\n", num);
//   //   printf("what is end?: %s", end);
//     int * address = (int *) num;
//     if ( (num == 0) && (end != str + strlen(str)   )){
//       shell_printf("error: peek cannot convert '%s'\n", str);
//       return 1;
//     }
//     else{
//       if ( (num & 0x3) != 0 ){
//         shell_printf("error: peek address must be 4-byte aligned\n");
//         return 1;
//       }
//       else{
//         //int * address = (int *) num;
//          //shell_printf("%08x: \n",  num);
//         //shell_printf("%08p: %08x\n", address, *address);
//           shell_printf("address is valid: %d\n", 4);
//           printf("done\n");
//         //shell_printf("lol\n");
//         return 0;
//       }
//     }
//   }
// //  return 1;
// }


int cmd_peek(int argc, const char* argv[]){
  if (argc < 2){
    shell_printf("error: peek expects 1 argument [address]\n");
    return 1;
  }
  else{
    const char * end = '\0';
    const char * str = argv[1];
    int num = strtonum(str, &end);
    printf( "what is end?: %s\n", end);
    printf("what is this string?:  %s", str + strlen(str) );
    if (  strlen(end) != 0 && !isdigit(end)){

      shell_printf("error: peek cannot convert '%s'\n", str);
      return 1;
    }
    else{
      if ( (num & 0x3) != 0 ){
        shell_printf("error: peek address must be 4-byte aligned\n");
        return 1;
      }
      else{
         char * address = num;
        shell_printf("%08p: %08x\n", address, *address);
        return 0;
      }
    }
  }
  return 1;
}

/*
 * See shell_commands.h for details on cmd_poke
 */
int cmd_poke(int argc, const char* argv[]){
  if (argc < 3){
    shell_printf("error: poke expects 2 argument [address] [value]\n");
    return 1;
  }
  else {
    const char * addr_end = '\0';
    const char * addr_str = argv[1];
    const char * val_end = '\0';
    const char * val_str = argv[2];
    int addr = strtonum(addr_str, &addr_end);
    int val = strtonum(val_str, &val_end);

    if (  strlen(addr_end) != 0 && !isdigit(addr_end)){
      shell_printf("error: poke cannot convert '%s'\n", addr_str);
      return 1;
    }
    else if  ( strlen(val_end) != 0 && !isdigit(val_end)){
      shell_printf("error: poke cannot convert '%s'\n", val_str);
      return 1;
    }
    else{
      if ( (addr & 0x3) != 0 ){
        shell_printf("error: poke address must be 4-byte aligned\n");
        return 1;
      }
      else{
        *(volatile int *) addr = val;
      }

    }
  }
  return 0;
}

/*
 * See shell.h for details on shell_init
 */
void shell_init(formatted_fn_t print_fn)
{
    shell_printf = print_fn;
}

/*
 * See shell.h for details on shell_bell
 */
void shell_bell(void)
{
    uart_putchar('\a');
}


/*
 * See shell.h for details on shell_readline
 */
void shell_readline(char buf[], int bufsize)
{
    int length = 0;
    while (1){
      unsigned c = keyboard_read_next();
      if (length == LINE_LEN -1) break;
      if ( c >= 32 && c <= 0x7F){
        buf[length++] = c;
        shell_printf((char *)&c);
      }
      if (c == '\n'){
        //buf[length++] = '\n';
        break;
      }
      if (c == '\b'){
        if (length <= 0){
          shell_bell();
        }
        else{
          shell_printf("\b");
          length--;
        }

      }

    }
    buf[length] = '\0';
    shell_printf("\n");

}

/*
 * See shell.h for details on shell_evaluate
 */

void clear_array( char * array[LINE_LEN], int ntokens){
  for (int i = 0; i < ntokens; i++){
    int len = strlen(array[i]);
    memset(array[i], 0, len);
  }
}



int shell_evaluate(const char *line)
{

    if (strlen(line) == 0){
      return 0;
    }

  //  printf("what does line contain in EVALUATE? :%s\n", line);
    char * array[LINE_LEN];
    memset(array, 0, LINE_LEN);
    int ntokens = tokenize(line, array, LINE_LEN);
    const char ** args = (const char **) (&array);
    int command_ref = isacommand( (const char *) array[0] );
    if (command_ref != -1){

      int a =  commands[command_ref].fn(ntokens, args);
      free(array);
      return 0;
    }
    else{

        shell_printf("error: no such command '%s'. Use 'help' for list of available commands.\n", array[0]);
        free(array);
        return 1;
    }

}

/*
 * See shell.h for details on shell_run
 */
void shell_run(void)
{
    shell_printf("Welcome to the CS107E shell. Remember to type on your PS/2 keyboard!\n");
    while (1)
    {
        char line[LINE_LEN];
        shell_printf("Pi> ");
        //timer_delay(1);
        //shell_printf("woo\n");
        shell_readline(line, sizeof(line));
      shell_evaluate(line);
    }
}

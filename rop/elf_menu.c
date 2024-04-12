#include <curses.h>
#include <stddef.h>
#include <stdio.h>

#include "elf_menu.h"
#include "my_elf.h"

typedef int (*MenuAction) (void *);
typedef struct _MenuItem
{
  const char *text;
  MenuAction action;
} MenuItem;

static int display_elf_header (void *);
static int disassemble_code_section (void *);
static int display_symbol_table (void *);
static int display_relocation_table (void *);
static int display_dynamic_symbol_table (void *);
static int display_dynamic_relocation_table (void *);
static int display_dynamic_section (void *);
static int display_program_header_table (void *);
static int display_section_header_table (void *);
static int display_string_table (void *);
static int display_all (void *);
static int exit_program (void *);

static void print_and_wait (const char *str);
static void init_menu (void);
static void cleanup_menu (void);
static void draw_menu (int highlight);

MenuItem menu_items[] = {
  { "Display elf header", display_elf_header },
  { "Disassemble code section", disassemble_code_section },
  { "Display symbol table", display_symbol_table },
  { "Display relocation table", display_relocation_table },
  { "Display dynamic symbol table", display_dynamic_symbol_table },
  { "Display dynamic relocation table", display_dynamic_relocation_table },
  { "Display dynamic section", display_dynamic_section },
  { "Display program header table", display_program_header_table },
  { "Display section header table", display_section_header_table },
  { "Display string table", display_string_table },
  { "Display all", display_all },
  { "Exit", exit_program },
};
int num_menu_items = sizeof (menu_items) / sizeof (MenuItem);

static void
init_menu (void)
{
  initscr ();
  cbreak ();
  noecho ();

  intrflush (stdscr, FALSE);
  keypad (stdscr, TRUE);
}

static void
cleanup_menu (void)
{
  endwin ();
}

static void
draw_menu (int highlight)
{
  for (int i = 0; i < num_menu_items; i++)
    {
      if (i == highlight)
        {
          attron (A_REVERSE);
        }
      mvprintw (i + 1, 1, "0x0%x) %s", i, menu_items[i].text);
      attroff (A_REVERSE);
    }
}

void
do_elf_menu (void)
{
  init_menu ();

  int highlight = 0;
  int choice = 0;

  while (1)
    {
      clear ();
      draw_menu (highlight);
      refresh ();

      choice = getch ();

      switch (choice)
        {
        case '0':
          highlight = 0;
          break;
        case '1':
          highlight = 1;
          break;
        case '2':
          highlight = 2;
          break;
        case '3':
          highlight = 3;
          break;
        case '4':
          highlight = 4;
          break;
        case '5':
          highlight = 5;
          break;
        case '6':
          highlight = 6;
          break;
        case '7':
          highlight = 7;
          break;
        case '8':
          highlight = 8;
          break;
        case '9':
          highlight = 9;
          break;
        case 'a':
          highlight = 10;
          break;
        case 'b':
          highlight = 11;
          break;
        case KEY_UP:
          highlight--;
          if (highlight < 0)
            highlight = 0;
          break;
        case KEY_DOWN:
          highlight++;
          if (highlight >= num_menu_items)
            highlight = num_menu_items - 1;
          break;
        default:
          break;
        }

      if (choice == 10)
        {
          int option = menu_items[highlight].action (NULL);
          if (option == 1)
            {
              break;
            }
        }
    }

  cleanup_menu ();
}

int
get_menu_input (void)
{
  int c = getchar ();

  if (c == EOF)
    {
      printf ("Error reading character.\n");
    }
  else
    {
      printf ("You entered: %c\n", (char)c);
    }

  return c;
}

int
display_elf_header (void *v)
{
  print_and_wait ("Display elf header\n");
  return 0;
}

int
disassemble_code_section (void *v)
{
  print_and_wait ("Disassemble code section\n");
  return 0;
}

int
display_symbol_table (void *v)
{
  print_and_wait ("Display symbol table\n");
  return 0;
}

int
display_relocation_table (void *v)
{
  print_and_wait ("Display relocation table\n");
  return 0;
}

int
display_dynamic_symbol_table (void *v)
{
  print_and_wait ("Display dynamic symbol table\n");
  return 0;
}

int
display_dynamic_relocation_table (void *v)
{
  print_and_wait ("Display dynamic relocation table\n");
  return 0;
}

int
display_dynamic_section (void *v)
{
  print_and_wait ("Display dynamic section\n");
  return 0;
}

int
display_program_header_table (void *v)
{
  print_and_wait ("Display program header table\n");
  return 0;
}

int
display_section_header_table (void *v)
{
  print_and_wait ("Display section header table\n");
  return 0;
}

int
display_string_table (void *v)
{
  print_and_wait ("Display string table\n");
  return 0;
}

int
display_all (void *v)
{
  print_and_wait ("Display all\n");
  return 0;
}

int
exit_program (void *v)
{
  print_and_wait ("Exiting program\n");
  return 1;
}

static void
print_and_wait (const char *str)
{
  clear ();
  printw (str);
  printw ("Press any key to continue: ");
  refresh ();
  (void)getch ();
}
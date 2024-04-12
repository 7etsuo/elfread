#include <curses.h>
#include <stddef.h>
#include <stdio.h>

#include "./include/elf_menu.h"
#include "./include/my_elf.h"
#include "./include/elf_menu.h"

int exit_program (void *);

static void init_screen (void);
static void cleanup_screen (void);
static void draw_menu (int highlight);
static int init_MenuAction (MenuConfig *config);

static const char *title;
static MenuItem menu_items[MAX_MENU_ITEMS] = { 0 };
static int num_menu_items = 0;

static void
init_screen (void)
{
  initscr ();
  cbreak ();
  noecho ();

  intrflush (stdscr, FALSE);
  keypad (stdscr, TRUE);
}

static void
cleanup_screen (void)
{
  endwin ();
}

static int
init_MenuAction (MenuConfig *config)
{
  for (size_t i = 0; i < config->item_count; i++)
    {
      if (config->items[i].text == NULL)
        {
          fprintf (stderr, "Menu item text is NULL.\n");
          return -1;
        }
      if (config->items[i].action == NULL)
        {
          fprintf (stderr, "Menu item action is NULL.\n");
          return -1;
        }

      menu_items[i] = config->items[i];
    }
  return 0;
}

static void
draw_menu (int highlight)
{
  if (title != NULL)
    {
      mvprintw (0, 0, title);
    }

  for (int i = 0; i < num_menu_items; i++)
    {
      if (i == highlight)
        {
          attron (A_REVERSE);
        }
      mvprintw (i + 1, 1, "0x0%x) %s %c", i, menu_items[i].text,
                i == num_menu_items - 1 ? '\n' : ' ');
      attroff (A_REVERSE);
    }
}

int
init_elf_menu (MenuConfig *config)
{
  if (config == NULL)
    {
      fprintf (stderr, "Menu configuration is NULL.\n");
      return -1;
    }

  if (config->item_count > MAX_MENU_ITEMS)
    {
      fprintf (stderr, "Too many menu items.\n");
      return -1;
    }

  title = config->title;
  int retval = init_MenuAction (config);
  if (retval != 0)
    {
      return retval;
    }

  num_menu_items = config->item_count;

  return 0;
}

void
do_elf_menu (void)
{
  init_screen ();

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
        case 'd':
          highlight = 12;
          break;
        case 'e':
          highlight = 13;
          break;
        case 'f':
          highlight = 14;
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

  cleanup_screen ();
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

void
menu_print (const char *str)
{
  printw (str);
}

void
print_and_wait (const char *str)
{
  clear ();
  printw (str);
  printw ("Press any key to continue: ");
  refresh ();
  (void)getch ();
}

#include <curses.h>
#include <stddef.h>
#include <stdio.h>

#include "./include/elf_menu.h"
#include "./include/my_elf.h"

int exit_program (void *);

static void init_screen (void);
static void cleanup_screen (void);
static void draw_menu (int highlight);
static int init_MenuAction (MenuConfig *config);

static MenuItem menu_items[MAX_MENU_ITEMS] = { 0 };
static int num_menu_items = 0;
static const char *title = NULL;
static void *data = NULL;

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
  if (config->item_count > MAX_MENU_ITEMS)
    {
      fprintf (stderr, "Too many menu items.\n");
      return -1;
    }
  num_menu_items = config->item_count;

  for (size_t i = 0; i < num_menu_items; i++)
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

  data = config->data;   // this is okay to be NULL
  title = config->title; // this is okay to be NULL

  return init_MenuAction (config) != 0;
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

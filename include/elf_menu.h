#ifndef ELF_MENU_H
#define ELF_MENU_H

#include <stddef.h>

#define MAX_MENU_ITEMS (0xf)

typedef int (*MenuAction) (void *);

typedef struct _MenuItem
{
  const char *text;
  MenuAction action;
} MenuItem;

typedef struct _MenuConfig
{
  const char *title;
  const MenuItem *items;
  size_t item_count;
} MenuConfig;

void menu_print (const char *str);
void print_and_wait (const char *str);
void do_elf_menu (void);
int init_elf_menu (MenuConfig *config);


#endif // ELF_MENU_H

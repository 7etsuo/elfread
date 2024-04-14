#if __APPLE__
#include <libelf/libelf.h>
#elif __linux__
#include <libelf.h>
#endif

#include "./include/elf_controller.h"
#include "./include/elf_menu.h"
#include "./include/fileio.h"
#include "./include/my_elf.h"

static void clean_controller (FileContents **filecontents);

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
clean_controller (FileContents **filecontents)
{
  robust_free ((*filecontents)->buffer);
  (*filecontents)->buffer = NULL;
  robust_free (*filecontents);
  *filecontents = NULL;
}

int
do_run_controller (const char *const filename)
{
  int retval = 1;

  FileContents *filecontents = robust_read_file (filename);
  if (filecontents == NULL)
    goto ret;

  Elf64_Ehdr ehdr = { 0 };
  if (get_elf_header (filecontents->buffer, filecontents->length, &ehdr) != 0)
    goto clean;

  MenuConfig config = { "ELF Menu", menu_items, num_menu_items };
  if (init_elf_menu (&config) != 0)
    goto clean;

  do_elf_menu ();

  retval = 0;

clean:
  clean_controller (&filecontents);

ret:
  return retval;
}

static int
display_elf_header (void *v)
{
  Elf64_Ehdr *ehdr = (Elf64_Ehdr *)v;

  // build strings for ehdr
  char *e_ident = malloc (EI_NIDENT + 1);
  if (e_ident == NULL)
    {
      fprintf (stderr, "Failed to allocate e_ident buffer\n");
      return -1;
    }
  strncpy (e_ident, ehdr->e_ident, EI_NIDENT);
  e_ident[EI_NIDENT] = '\0';

  char e_ident_str[50];
  snprintf (e_ident_str, sizeof (e_ident_str), "e_ident: %s\n", e_ident);

  char e_type_str[50];
  snprintf (e_type_str, sizeof (e_type_str), "e_type: %d\n", ehdr->e_type);

  char e_machine_str[50];
  snprintf (e_machine_str, sizeof (e_machine_str), "e_machine: %d\n",
            ehdr->e_machine);

  char e_version_str[50];
  snprintf (e_version_str, sizeof (e_version_str), "e_version: %d\n",
            ehdr->e_version);

  char e_entry_str[50];
  snprintf (e_entry_str, sizeof (e_entry_str), "e_entry: %lx\n",
            ehdr->e_entry);

  char e_phoff_str[50];
  snprintf (e_phoff_str, sizeof (e_phoff_str), "e_phoff: %lx\n",
            ehdr->e_phoff);

  char e_shoff_str[50];
  snprintf (e_shoff_str, sizeof (e_shoff_str), "e_shoff: %lx\n",
            ehdr->e_shoff);

  char e_flags_str[50];
  snprintf (e_flags_str, sizeof (e_flags_str), "e_flags: %x\n", ehdr->e_flags);

  char e_ehsize_str[50];
  snprintf (e_ehsize_str, sizeof (e_ehsize_str), "e_ehsize: %d\n",
            ehdr->e_ehsize);

  char e_phentsize_str[50];
  snprintf (e_phentsize_str, sizeof (e_phentsize_str), "e_phentsize: %d\n",
            ehdr->e_phentsize);

  char e_phnum_str[50];
  snprintf (e_phnum_str, sizeof (e_phnum_str), "e_phnum: %d\n", ehdr->e_phnum);

  char e_shentsize_str[50];
  snprintf (e_shentsize_str, sizeof (e_shentsize_str), "e_shentsize: %d\n",
            ehdr->e_shentsize);

  char e_shnum_str[50];
  snprintf (e_shnum_str, sizeof (e_shnum_str), "e_shnum: %d\n", ehdr->e_shnum);

  char e_shstrndx_str[50];
  snprintf (e_shstrndx_str, sizeof (e_shstrndx_str), "e_shstrndx: %d\n",
            ehdr->e_shstrndx);

  print_and_wait ("Display elf header\n");
  return 0;
}

static int
disassemble_code_section (void *v)
{
  print_and_wait ("Disassemble code section\n");
  return 0;
}

static int
display_symbol_table (void *v)
{
  print_and_wait ("Display symbol table\n");
  return 0;
}

static int
display_relocation_table (void *v)
{
  print_and_wait ("Display relocation table\n");
  return 0;
}

static int
display_dynamic_symbol_table (void *v)
{
  print_and_wait ("Display dynamic symbol table\n");
  return 0;
}

static int
display_dynamic_relocation_table (void *v)
{
  print_and_wait ("Display dynamic relocation table\n");
  return 0;
}

static int
display_dynamic_section (void *v)
{
  print_and_wait ("Display dynamic section\n");
  return 0;
}

static int
display_program_header_table (void *v)
{
  print_and_wait ("Display program header table\n");
  return 0;
}

static int
display_section_header_table (void *v)
{
  print_and_wait ("Display section header table\n");
  return 0;
}

static int
display_string_table (void *v)
{
  print_and_wait ("Display string table\n");
  return 0;
}

static int
display_all (void *v)
{
  print_and_wait ("Display all\n");
  return 0;
}

static int
exit_program (void *v)
{
  print_and_wait ("Exiting program\n");
  return 1;
}

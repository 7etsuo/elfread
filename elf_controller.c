#if __APPLE__
#include <libelf/libelf.h>
#elif __linux__
#include <libelf.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./include/elf_controller.h"
#include "./include/elf_menu.h"
#include "./include/fileio.h"
#include "./include/my_elf.h"

#define SIZE_TEMPBUF 100

const char *elf_class_id[] = {
#include "./include/e_class_strings.h"
};
const char *elf_data_id[] = {
#include "./include/e_data_strings.h"
};
const char *elf_osabi_id[] = {
#include "./include/e_osabi_strings.h"
};
const char *elf_e_type_id[] = {
#include "./include/e_type_strings.h"
};
const char *elf_e_machine_id[] = {
#include "./include/e_machine_strings.h"
};
const char *elf_e_version_id[] = {
#include "./include/e_version_strings.h"
};
const char *elf_p_type_id[] = {
#include "./include/p_type_strings.h"
};
const char *elf_s_type_id[] = {
#include "./include/s_type_strings.h"
};

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

  MenuConfig config = { "ELF Menu", menu_items, &ehdr, num_menu_items };
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

  char magic_str[SIZE_TEMPBUF];
  snprintf (magic_str, sizeof (magic_str), "Magic:   ");
  for (int i = 0; i < EI_NIDENT; i++)
    {
      char temp[4];
      snprintf (temp, sizeof (temp), "%02x ", ehdr->e_ident[i]);
      strncat (magic_str, temp, sizeof (magic_str) - strlen (magic_str) - 1);
    }
  strncat (magic_str, "\n", sizeof (magic_str) - strlen (magic_str) - 1);

  char class_str[SIZE_TEMPBUF];
  snprintf (class_str, sizeof (class_str),
            "Class:                             %s\n",
            elf_class_id[ehdr->e_ident[EI_CLASS]]);

  char data_str[SIZE_TEMPBUF];
  snprintf (data_str, sizeof (data_str),
            "Data:                              %s\n",
            elf_data_id[ehdr->e_ident[EI_DATA]]);

  char version_str[SIZE_TEMPBUF];
  snprintf (version_str, sizeof (version_str),
            "Version:                           %d %s\n",
            ehdr->e_ident[EI_VERSION],
            ehdr->e_ident[EI_VERSION] ? "(current)" : "(invalid)");

  char osabi_str[SIZE_TEMPBUF];
  snprintf (osabi_str, sizeof (osabi_str),
            "OS/ABI:                            %s\n",
            elf_osabi_id[ehdr->e_ident[EI_OSABI]]);

  char abiversion_str[SIZE_TEMPBUF];
  snprintf (abiversion_str, sizeof (abiversion_str),
            "ABI Version:                       %d\n",
            ehdr->e_ident[EI_ABIVERSION]);

  char type_str[SIZE_TEMPBUF];
  snprintf (type_str, sizeof (type_str),
            "Type:                              %s\n",
            elf_e_type_id[ehdr->e_type]);

  char machine_str[SIZE_TEMPBUF];
  snprintf (machine_str, sizeof (machine_str),
            "Machine:                           %s\n",
            elf_e_machine_id[ehdr->e_machine]);

  char version_str2[SIZE_TEMPBUF];
  snprintf (version_str2, sizeof (version_str2),
            "Version:                           0x%x %s\n", ehdr->e_version,
            elf_e_version_id[ehdr->e_version]);

  char entry_str[SIZE_TEMPBUF];
  snprintf (entry_str, sizeof (entry_str),
            "Entry point address:               0x%lx\n", ehdr->e_entry);

  char phoff_str[SIZE_TEMPBUF];
  snprintf (phoff_str, sizeof (phoff_str),
            "Start of program headers:          %ld (bytes into file)\n",
            ehdr->e_phoff);

  char shoff_str[SIZE_TEMPBUF];
  snprintf (shoff_str, sizeof (shoff_str),
            "Start of section headers:          %ld (bytes into file)\n",
            ehdr->e_shoff);

  char flags_str[SIZE_TEMPBUF];
  snprintf (flags_str, sizeof (flags_str),
            "Flags:                             0x%x\n", ehdr->e_flags);

  char ehsize_str[SIZE_TEMPBUF];
  snprintf (ehsize_str, sizeof (ehsize_str),
            "Size of this header:               %d (bytes)\n", ehdr->e_ehsize);

  char phentsize_str[SIZE_TEMPBUF];
  snprintf (phentsize_str, sizeof (phentsize_str),
            "Size of program headers:           %d (bytes)\n",
            ehdr->e_phentsize);

  char phnum_str[SIZE_TEMPBUF];
  snprintf (phnum_str, sizeof (phnum_str),
            "Number of program headers:         %d\n", ehdr->e_phnum);

  char shentsize_str[SIZE_TEMPBUF];
  snprintf (shentsize_str, sizeof (shentsize_str),
            "Size of section headers:           %d (bytes)\n",
            ehdr->e_shentsize);

  char shnum_str[SIZE_TEMPBUF];
  snprintf (shnum_str, sizeof (shnum_str),
            "Number of section headers:         %d\n", ehdr->e_shnum);

  char shstrndx_str[SIZE_TEMPBUF];
  snprintf (shstrndx_str, sizeof (shstrndx_str),
            "Section header string table index: %d\n", ehdr->e_shstrndx);

  elfprint (magic_str);
  elfprint (class_str);
  elfprint (data_str);
  elfprint (version_str);
  elfprint (osabi_str);
  elfprint (abiversion_str);
  elfprint (type_str);
  elfprint (machine_str);
  elfprint (version_str2);
  elfprint (entry_str);
  elfprint (phoff_str);
  elfprint (shoff_str);
  elfprint (flags_str);
  elfprint (ehsize_str);
  elfprint (phentsize_str);
  elfprint (phnum_str);
  elfprint (shentsize_str);
  elfprint (shnum_str);
  elfprint (shstrndx_str);

  print_and_wait ("\n");
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

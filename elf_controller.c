#if __APPLE__
#include <libelf/libelf.h>
#elif __linux__
#include <libelf.h>
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "./include/elf_controller.h"
#include "./include/elf_menu.h"
#include "./include/fileio.h"
#include "./include/my_elf.h"

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
  { "Display program header table", display_program_header_table },
  { "Display section header table", display_section_header_table },
  { "Disassemble code section", disassemble_code_section },
  { "Display symbol table", display_symbol_table },
  { "Display relocation table", display_relocation_table },
  { "Display dynamic symbol table", display_dynamic_symbol_table },
  { "Display dynamic relocation table", display_dynamic_relocation_table },
  { "Display dynamic section", display_dynamic_section },
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

  MenuConfig config = { "ELF Menu", menu_items, filecontents, num_menu_items };
  if (init_elf_menu (&config) != 0)
    goto clean;

  do_elf_menu ();

  retval = 0;

clean:
  clean_controller (&filecontents);

ret:
  return retval;
}

int
format_and_print (const char *label, const char *format, ...)
{
  if (label == NULL || format == NULL)
    {
      return 1;
    }

  if (strlen (label) + strlen (format) + 1 > SIZE_TEMPBUF)
    {
      return 1;
    }

  char buf[SIZE_TEMPBUF];
  va_list args;

  snprintf (buf, sizeof (buf), "%s", label);
  size_t available_space = sizeof (buf) - strlen (buf) - 1; // -1 for '\0'
  va_start (args, format);
  vsnprintf (buf + strlen (buf), available_space, format, args);
  va_end (args);

  elfprint (buf);

  return 0;
}

static int
display_elf_header (void *v)
{
  FileContents *filecontents = (FileContents *)v;

  Elf64_Ehdr ehdr = { 0 };
  if (get_elf_header (filecontents->buffer, filecontents->length, &ehdr) != 0)
    {
      print_and_wait ("Failed to get ELF header\n");
      return 1;
    }

  format_and_print ("Magic:   ", "");
  for (int i = 0; i < EI_NIDENT; i++)
    {
      format_and_print ("", "%02x ", ehdr.e_ident[i]);
      if (i == EI_NIDENT - 1)
        {
          elfprint ("\n");
        }
    }

  format_and_print ("Class:                             %s\n",
                    elf_class_id[ehdr.e_ident[EI_CLASS]]);

  char data_str[SIZE_TEMPBUF];
  snprintf (data_str, sizeof (data_str),
            "Data:                              %s\n",
            elf_data_id[ehdr.e_ident[EI_DATA]]);

  char version_str[SIZE_TEMPBUF];
  snprintf (version_str, sizeof (version_str),
            "Version:                           %d %s\n",
            ehdr.e_ident[EI_VERSION],
            ehdr.e_ident[EI_VERSION] ? "(current)" : "(invalid)");

  char osabi_str[SIZE_TEMPBUF];
  snprintf (osabi_str, sizeof (osabi_str),
            "OS/ABI:                            %s\n",
            elf_osabi_id[ehdr.e_ident[EI_OSABI]]);

  char abiversion_str[SIZE_TEMPBUF];
  snprintf (abiversion_str, sizeof (abiversion_str),
            "ABI Version:                       %d\n",
            ehdr.e_ident[EI_ABIVERSION]);

  char type_str[SIZE_TEMPBUF];
  snprintf (type_str, sizeof (type_str),
            "Type:                              %s\n",
            elf_e_type_id[ehdr.e_type]);

  char machine_str[SIZE_TEMPBUF];
  snprintf (machine_str, sizeof (machine_str),
            "Machine:                           %s\n",
            elf_e_machine_id[ehdr.e_machine]);

  char version_str2[SIZE_TEMPBUF];
  snprintf (version_str2, sizeof (version_str2),
            "Version:                           0x%x %s\n", ehdr.e_version,
            elf_e_version_id[ehdr.e_version]);

  char entry_str[SIZE_TEMPBUF];
  snprintf (entry_str, sizeof (entry_str),
            "Entry point address:               0x%lx\n", ehdr.e_entry);

  char phoff_str[SIZE_TEMPBUF];
  snprintf (phoff_str, sizeof (phoff_str),
            "Start of program headers:          %ld (bytes into file)\n",
            ehdr.e_phoff);

  char shoff_str[SIZE_TEMPBUF];
  snprintf (shoff_str, sizeof (shoff_str),
            "Start of section headers:          %ld (bytes into file)\n",
            ehdr.e_shoff);

  char flags_str[SIZE_TEMPBUF];
  snprintf (flags_str, sizeof (flags_str),
            "Flags:                             0x%x\n", ehdr.e_flags);

  char ehsize_str[SIZE_TEMPBUF];
  snprintf (ehsize_str, sizeof (ehsize_str),
            "Size of this header:               %d (bytes)\n", ehdr.e_ehsize);

  char phentsize_str[SIZE_TEMPBUF];
  snprintf (phentsize_str, sizeof (phentsize_str),
            "Size of program headers:           %d (bytes)\n",
            ehdr.e_phentsize);

  char phnum_str[SIZE_TEMPBUF];
  snprintf (phnum_str, sizeof (phnum_str),
            "Number of program headers:         %d\n", ehdr.e_phnum);

  char shentsize_str[SIZE_TEMPBUF];
  snprintf (shentsize_str, sizeof (shentsize_str),
            "Size of section headers:           %d (bytes)\n",
            ehdr.e_shentsize);

  char shnum_str[SIZE_TEMPBUF];
  snprintf (shnum_str, sizeof (shnum_str),
            "Number of section headers:         %d\n", ehdr.e_shnum);

  char shstrndx_str[SIZE_TEMPBUF];
  snprintf (shstrndx_str, sizeof (shstrndx_str),
            "Section header string table index: %d\n", ehdr.e_shstrndx);

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
  elfprint (shstrndx_str);

  print_and_wait ("\n");
  return 0;
}

static void
print_phdr_main_header_titles (void)
{
  char header[1000];

  snprintf (header, sizeof (header), PHDR_MAIN_HEADER_TITLES_FORMAT, "Type",
            "Offset", "VirtAddr", "PhysAddr");
  snprintf (header + strlen (header), sizeof (header) - strlen (header),
            PHDR_SUBHEADER_TITLES_FORMAT, "FileSiz", "MemSiz", "Flags",
            "Align");
  elfprint (header);
}

static int
display_program_header_table (void *v)
{
  FileContents *filecontents = (FileContents *)v;

  Elf64_Ehdr ehdr = { 0 };
  if (get_elf_header (filecontents->buffer, filecontents->length, &ehdr) != 0)
    {
      print_and_wait ("Failed to get ELF header\n");
      return 1;
    }

  print_phdr_main_header_titles ();

  Elf64_Phdr phdr[ehdr.e_phnum];
  memset (phdr, 0, sizeof (phdr));

  for (int i = 0; i < ehdr.e_phnum; i++)
    {
      get_elf_phdr (filecontents->buffer, i * ehdr.e_phentsize, &ehdr,
                    &(phdr[i]));

      char buf[1000] = { 0 };
      snprintf (buf, sizeof (buf), "[%3d] %-14s 0x%016lx 0x%016lx 0x%016lx\n",
                i, get_p_type (phdr[i].p_type), phdr[i].p_offset,
                phdr[i].p_vaddr, phdr[i].p_paddr);
      elfprint (buf);

      char flags_buf[4] = { 0 };
      snprintf (buf, sizeof (buf), "%-20s 0x%016lx 0x%016lx %-6s 0x%06lx\n",
                " ", phdr[i].p_filesz, phdr[i].p_memsz,
                get_p_flags (phdr[i].p_flags, flags_buf), phdr[i].p_align);
      elfprint (buf);
    }

  // [TODO]: Implement
  // .interp section is a null-terminated string that specifies the path name
  // of the interpreter.
  //   Elf64_Shdr interp_section;
  //   if (get_elf_shdr (filecontents->buffer,
  //                     ehdr.e_shoff + ehdr.e_shentsize * ehdr.e_shstrndx,
  //                     &ehdr, &interp_section)
  //       != 0)
  //     {
  //       print_and_wait ("Failed to get INTERP section\n");
  //       return 1;
  //     }
  //
  //   char interp_path[interp_section.sh_size];
  //   memcpy (interp_path, filecontents->buffer + interp_section.sh_offset,
  //           interp_section.sh_size);
  //   interp_path[interp_section.sh_size - 1] = '\0';
  //
  //   print_and_wait ("Interpreter path: ");
  //   elfprint (interp_path);
  //   print_and_wait ("\n");

  print_and_wait ("\n");
  return 0;
}

static int
display_section_header_table (void *v)
{
  print_and_wait ("Display section header table\n");
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

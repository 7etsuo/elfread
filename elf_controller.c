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

#define err_exit(msg)                                                         \
  do                                                                          \
    {                                                                         \
      perror (msg);                                                           \
      exit (EXIT_FAILURE);                                                    \
    }                                                                         \
  while (0);
#define INDEX_ET_OS 6
#define INDEX_ET_PROC 7
#define ARRAY_SIZE(arr) (sizeof (arr) / sizeof ((arr)[0]))
#define STRTABLE_MAX 255

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

static void print_phdr_main_header_titles (void);
static void clean_controller (FileContents **filecontents);

// elf header
static int display_elf_header (void *);
static void print_elf_header (const Elf64_Ehdr *ehdr);
static Elf64_Half emit_e_type (const Elf64_Ehdr *ehdr);
static Elf64_Half emit_ei_class (const Elf64_Ehdr *ehdr);
static Elf64_Half emit_ei_data (const Elf64_Ehdr *ehdr);
static Elf64_Half emit_ei_osabi (const Elf64_Ehdr *ehdr);
static Elf64_Half emit_e_type (const Elf64_Ehdr *ehdr);

static int display_program_header_table (void *);
static int display_section_header_table (void *);

static int disassemble_code_section (void *);
static int display_symbol_table (void *);
static int display_relocation_table (void *);
static int display_dynamic_symbol_table (void *);
static int display_dynamic_relocation_table (void *);
static int display_dynamic_section (void *);
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

static Elf64_Half
emit_ei_class (const Elf64_Ehdr *ehdr)
{
  Elf64_Half elf_ei_class = ehdr->e_ident[EI_CLASS];
  if (elf_ei_class < ELFCLASS32 || elf_ei_class > ELFCLASS64)
    elf_ei_class = ELFCLASSNONE;

  return elf_ei_class;
}

static Elf64_Half
emit_ei_data (const Elf64_Ehdr *ehdr)
{
  Elf64_Half elf_ei_data = ehdr->e_ident[EI_DATA];
  if (elf_ei_data < ELFDATA2LSB || elf_ei_data > ELFDATA2MSB)
    elf_ei_data = ELFDATANONE;

  return elf_ei_data;
}

static Elf64_Half
emit_ei_osabi (const Elf64_Ehdr *ehdr)
{
  Elf64_Half elf_ei_osabi = ehdr->e_ident[EI_OSABI];
  if (elf_ei_osabi >= ELFOSABI_SOLARIS && elf_ei_osabi <= ELFOSABI_OPENBSD)
    elf_ei_osabi -= 2;
  else if (elf_ei_osabi >= ELFOSABI_ARM_AEABI)
    elf_ei_osabi = (ARRAY_SIZE (elf_osabi_id) - 1);

  return elf_ei_osabi;
}

static Elf64_Half
emit_e_type (const Elf64_Ehdr *ehdr)
{
  Elf64_Half elf_e_type = ehdr->e_type;
  if (elf_e_type > 5 && elf_e_type < ET_LOOS)
    elf_e_type = ET_NONE;
  else if (elf_e_type >= ET_LOOS && elf_e_type <= ET_HIOS)
    elf_e_type = INDEX_ET_OS;
  else if (elf_e_type >= ET_HIOS && elf_e_type <= ET_LOPROC)
    elf_e_type = INDEX_ET_PROC;

  return elf_e_type;
}

static void
print_elf_header (const Elf64_Ehdr *ehdr)
{
  if (!ehdr)
    {
      fprintf (stderr, "Null pointer passed to print_elf_header\n");
      return;
    }

  Elf64_Half elf_ei_class = emit_ei_class (ehdr);
  Elf64_Half elf_ei_data = emit_ei_data (ehdr);
  Elf64_Half elf_ei_osabi = emit_ei_osabi (ehdr);
  Elf64_Half elf_e_type = emit_e_type (ehdr);

  char buffer[1024]; // Increase this if necessary
  char *buf_ptr = buffer;

  buf_ptr += snprintf (buf_ptr, sizeof (buffer) - (buf_ptr - buffer),
                       "ELF Header:\n  Magic:   ");

  for (int i = 0; i < EI_NIDENT && buf_ptr - buffer < sizeof (buffer); i++)
    buf_ptr += snprintf (buf_ptr, sizeof (buffer) - (buf_ptr - buffer),
                         "%.2x ", ehdr->e_ident[i]);

  buf_ptr += snprintf (
      buf_ptr, sizeof (buffer) - (buf_ptr - buffer),
      "\n"
      "  Class:                               %s\n"
      "  Data:                                %s\n"
      "  Version:                             %d\n"
      "  OS/ABI:                              %s\n"
      "  ABI Version:                         %d\n"
      "  Type:                                %s\n"
      "  Machine:                             %s\n"
      "  Version:                             0x%x (%s)\n"
      "  Entry point address:                 0x%x\n"
      "  Start of program headers:            %d (bytes into file)\n"
      "  Start of section headers:            %d (bytes into file)\n"
      "  Flags:                               0x%x\n"
      "  Size of this header:                 %d (bytes)\n"
      "  Size of program headers:             %d (bytes)\n"
      "  Number of program headers:           %d\n"
      "  Size of section headers:             %d (bytes)\n"
      "  Number of section headers:           %d\n"
      "  Section header string table index:   %d\n",
      elf_class_id[elf_ei_class], elf_data_id[elf_ei_data],
      (int)ehdr->e_ident[EI_VERSION], elf_osabi_id[elf_ei_osabi],
      (int)ehdr->e_ident[EI_ABIVERSION], elf_e_type_id[elf_e_type],
      ehdr->e_machine >= EM_NUM ? "special\n"
                                : elf_e_machine_id[ehdr->e_machine],
      ehdr->e_version, elf_e_version_id[ehdr->e_version], (int)ehdr->e_entry,
      (int)ehdr->e_phoff, (int)ehdr->e_shoff, (int)ehdr->e_flags,
      (int)ehdr->e_ehsize, (int)ehdr->e_phentsize, (int)ehdr->e_phnum,
      (int)ehdr->e_shentsize, (int)ehdr->e_shnum, (int)ehdr->e_shstrndx);

  elfprint (buffer);
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

  print_elf_header (&ehdr);
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

      format_and_print ("", "[%3d] %-14s 0x%016lx 0x%016lx 0x%016lx\n", i,
                        get_p_type (phdr[i].p_type), phdr[i].p_offset,
                        phdr[i].p_vaddr, phdr[i].p_paddr);

      char flags_buf[4] = { 0 };
      format_and_print ("", "%-20s 0x%016lx 0x%016lx %-6s 0x%06lx\n", " ",
                        phdr[i].p_filesz, phdr[i].p_memsz,
                        get_p_flags (phdr[i].p_flags, flags_buf),
                        phdr[i].p_align);
    }

#ifdef INTERP
  // [TODO]: Implement
  // .interp section is a null-terminated string that specifies the path name
  // of the interpreter.

  Elf64_Shdr interp_section;
  if (get_elf_shdr (filecontents->buffer,
                    ehdr.e_shoff + ehdr.e_shentsize * ehdr.e_shstrndx, &ehdr,
                    &interp_section)
      != 0)
    {
      print_and_wait ("Failed to get INTERP section\n");
      return 1;
    }

  char interp_path[interp_section.sh_size];
  memcpy (interp_path, filecontents->buffer + interp_section.sh_offset,
          interp_section.sh_size);
  interp_path[interp_section.sh_size - 1] = '\0';

  print_and_wait ("Interpreter path: ");
  elfprint (interp_path);
  print_and_wait ("\n");
#endif // INTERP

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

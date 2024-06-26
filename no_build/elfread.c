/* elfread.c
 * usage:
 * $ gcc elfread.c -o elfread -lm
 * $ ./elfread -hlS /bin/ls
 *
 */

#ifdef __APPLE__
#include <libelf/libelf.h>
#elif __linux__
#include <libelf.h>
#endif

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "./../include/strings_global.h"

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

const char *g_help_menu
    = { "Usage: elfread <option(s)> elf-file(s)\n"
        " Display information about the contents of ELF format files\n"
        " Options are:\n"
        "\n"
        "-h --file-header               Display the ELF file header\n"
        "-l --program-headers           Display the program headers\n"
        "   --segments                                             \n"
        "-S --section-headers           Displays the information contained \n"
        "   --sections                  in the file's section headers\n"
        "-e --headers                   Display all the headers -hlS\n"
        "-H --help                      Display this information\n\n"
        "                               the-scientist@rootstorm.com\n"
        "                               https://www.rootstorm.com\n\n" };
const char *elf_class_id[ELFCLASSNUM] = {
#include "./include/e_class_strings.h"
};
const char *elf_data_id[ELFDATANUM] = {
#include "./include/e_data_strings.h"
};
const char *elf_osabi_id[] = {
#include "./include/e_osabi_strings.h"
};
const char *elf_e_type_id[] = {
#include "./include/e_type_strings.h"
};
const char *elf_e_machine_id[EM_NUM] = {
#include "./include/e_machine_strings.h"
};
const char *elf_e_version_id[EV_NUM] = {
#include "./include/e_version_strings.h"
};
const char *elf_s_type_id[] = {
#include "./include/s_type_strings.h"
};

static int g_elf_file_header_flag = 0;
static int g_elf_prog_header_flag = 0;
static int g_elf_section_header_flag = 0;
static int g_elf_help_flag = 0;

void display_elf_s_section_header (const Elf64_Shdr *section,
                                   const Elf64_Ehdr *ehdr, const void *data);
void display_elf_p_segment_header (const Elf64_Phdr *segment,
                                   const Elf64_Ehdr *ehdr, const void *data);
int read_file_into_mem (const char *filename, void **data_out,
                        size_t *size_out);
int write_mem_to_file (const char *filename, const void *data, size_t size);
void display_elf_header (const Elf64_Ehdr *ehdr);
int get_s_type_index (Elf64_Word type);
int get_p_type_index (Elf64_Word type);
Elf64_Half emit_e_type (const Elf64_Ehdr *ehdr);
Elf64_Half emit_ei_class (const Elf64_Ehdr *ehdr);
Elf64_Half emit_ei_data (const Elf64_Ehdr *ehdr);
Elf64_Half emit_ei_osabi (const Elf64_Ehdr *ehdr);
Elf64_Half emit_e_type (const Elf64_Ehdr *ehdr);
Elf64_Word emit_e_version (const Elf64_Ehdr *ehdr);
void get_segment_pflag (char *flagstring, Elf64_Word flags);
int get_string_table (const char *stringtable[], Elf64_Off stroff,
                      Elf64_Xword sh_sz, const void *data);

#if TEST_ELFREAD == 1
int
main (int argc, char **argv)
{
  void *data;
  const char *binpath;
  int c, option_index;
  size_t datasz;
  Elf64_Ehdr ehdr;
  Elf64_Phdr segment[PN_XNUM];
  Elf64_Shdr section[SHN_LORESERVE];

  while (1)
    {
      option_index = 0;
      static struct option long_options[]
          = { { "file-header", no_argument, 0, 'h' },
              { "program-headers", no_argument, 0, 'l' },
              { "segments", no_argument, 0, 'l' },
              { "section-headers", no_argument, 0, 'S' },
              { "sections", no_argument, 0, 'S' },
              { "headers", no_argument, 0, 'e' },
              { "help", no_argument, 0, 'H' },
              { 0, 0, 0, 0 } };

      c = getopt_long (argc, argv, "hlSeH", long_options, &option_index);
      if (c == -1)
        {
          g_elf_help_flag = optind == 1 ? optind : g_elf_help_flag;
          break;
        }

      switch (c)
        {
        case 'h':
          g_elf_file_header_flag = 1;
          break;
        case 'l':
          g_elf_prog_header_flag = 1;
          break;
        case 'S':
          g_elf_section_header_flag = 1;
          break;
        case 'e':
          g_elf_file_header_flag = 1;
          g_elf_prog_header_flag = 1;
          g_elf_section_header_flag = 1;
          break;
        case 'H':
        case '?':
        default:
          g_elf_help_flag = 1;
        }
    }

  if (optind + 1 == argc)
    binpath = argv[optind];
  else
    g_elf_help_flag = 1;

  if (g_elf_help_flag)
    err_exit (g_help_menu);

  if (read_file_into_mem (binpath, &data, &datasz) == 0)
    err_exit ("* read_file_into_mem() error");

  if (datasz < sizeof (Elf64_Ehdr))
    err_exit ("* not an ordinary file");

  memcpy (&ehdr, data, sizeof (Elf64_Ehdr));
  memcpy (&segment, data + ehdr.e_phoff, ehdr.e_phentsize * ehdr.e_phnum);
  memcpy (&section, data + ehdr.e_shoff, ehdr.e_shentsize * ehdr.e_shnum);

  if (strncmp (ELFMAG, (const char *)&ehdr.e_ident[EI_MAG0], SELFMAG) != 0)
    err_exit ("* Error: Not an ELF file"
              "- it has the wrong magic bytes at the start");

  if (g_elf_file_header_flag)
    display_elf_header (&ehdr);

  if (g_elf_prog_header_flag)
    display_elf_p_segment_header (segment, &ehdr, data);

  if (g_elf_section_header_flag)
    display_elf_s_section_header (section, &ehdr, data);

  free (data);
  return 0;
}
#endif // TEST_ELFREAD

void
display_elf_s_section_header (const Elf64_Shdr *section,
                              const Elf64_Ehdr *ehdr, const void *data)
{
  Elf64_Off stroff = section[ehdr->e_shstrndx].sh_offset;
  int nrsz = (int)log10 ((double)ehdr->e_shnum) + 1;
  char *nr = "Nr";
  char *spc = " ";

  printf (
      "There are %d section headers, starting at offset 0x%.4x:\n"
      "\n"
      "Section Headers:\n"
      "[%*s] Name                 Type             Address          Offset\n"
      " %*s  Size                 EntSize          Flags            Link  "
      "Info  Align\n",
      (int)ehdr->e_shnum, ehdr->e_shoff, nrsz, nr, nrsz, spc);

  for (int i = 0; i < ehdr->e_shnum; i++)
    {
      printf ("[%*d] %-*s %-*s %-.*x %-.*x\n"
              " %*s  %-.*x     %-.*x %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c %4d  "
              "%4d  %5d\n",
              nrsz, i, 20, (data + stroff + section[i].sh_name), 16,
              elf_s_type_id[get_s_type_index (section[i].sh_type)], 16,
              section[i].sh_addr, 16, section[i].sh_offset, nrsz, spc, 16,
              section[i].sh_size, 16, section[i].sh_entsize,
              (section[i].sh_flags & SHF_WRITE ? 'W' : ' '),
              (section[i].sh_flags & SHF_ALLOC ? 'A' : ' '),
              (section[i].sh_flags & SHF_EXECINSTR ? 'X' : ' '),
              (section[i].sh_flags & SHF_MERGE ? 'M' : ' '),
              (section[i].sh_flags & SHF_STRINGS ? 'S' : ' '),
              (section[i].sh_flags & SHF_INFO_LINK ? 'I' : ' '),
              (section[i].sh_flags & SHF_LINK_ORDER ? 'L' : ' '),
              (section[i].sh_flags & SHF_OS_NONCONFORMING ? 'O' : ' '),
              (section[i].sh_flags & SHF_GROUP ? 'G' : ' '),
              (section[i].sh_flags & SHF_TLS ? 'T' : ' '),
              (section[i].sh_flags & SHF_COMPRESSED ? 'C' : ' '),
              (section[i].sh_flags & (1 << 12) ? 'x' : ' '),
              (section[i].sh_flags & SHF_MASKOS ? 'o' : ' '),
              (section[i].sh_flags & SHF_EXCLUDE ? 'E' : ' '),
              (section[i].sh_flags & SHF_ORDERED ? 'l' : ' '),
              (section[i].sh_flags & SHF_MASKPROC ? 'p' : ' '),
              section[i].sh_link, section[i].sh_info, section[i].sh_addralign);
    }

  printf ("Key to Flags:\n"
          "  W (write), A (alloc), X (execute), M (merge), S (strings), I "
          "(info),\n"
          "  L (link order), O (extra OS processing required), G (group), T "
          "(TLS),\n"
          "  C (compressed), x (unknown), o (OS specific), E (exclude),\n"
          "  l (large), p (processor specific)\n\n");
}

void
display_elf_p_segment_header (const Elf64_Phdr *segment,
                              const Elf64_Ehdr *ehdr, const void *data)
{
  Elf64_Half elf_e_type = emit_e_type (ehdr);
  off_t p_type_offs;

  printf ("\nElf file type is %s\n"
          "Entry point 0x%x\n"
          "There are %d program headers, starting at offset %d\n"
          "\n",
          elf_e_type_id[elf_e_type], (int)ehdr->e_entry, (int)ehdr->e_phnum,
          (int)ehdr->e_phentsize);

  printf ("Program Headers:\n"
          "  Type           Offset             VirtAddr           PhysAddr    "
          "      \n"
          "                 FileSiz            MemSiz              Flags  "
          "Align     \n");

  for (size_t i = 0; i < ehdr->e_phnum; i++)
    {
      p_type_offs = get_p_type_index (segment[i].p_type);
      printf ("  %-12s\t 0x%.16x 0x%.16x 0x%.16x\n"
              "                 0x%.16x 0x%.16x  %c%c%c    0x%x\n",
              elf_p_type_id[p_type_offs], segment[i].p_offset,
              segment[i].p_vaddr, segment[i].p_paddr, segment[i].p_filesz,
              segment[i].p_memsz, (segment[i].p_flags & PF_R ? 'R' : ' '),
              (segment[i].p_flags & PF_W ? 'W' : ' '),
              (segment[i].p_flags & PF_X ? 'E' : ' '), segment[i].p_align);

      if (segment[i].p_type == PT_INTERP)
        printf ("      [Requesting program interpreter: %s]\n",
                data + segment[i].p_offset);
    }

  putchar ('\n');
  printf (" Section to Segment mapping:\n"
          "  Segment Sections...\n");
}

Elf64_Half
emit_ei_class (const Elf64_Ehdr *ehdr)
{
  Elf64_Half elf_ei_class = ehdr->e_ident[EI_CLASS];
  if (elf_ei_class < ELFCLASS32 || elf_ei_class > ELFCLASS64)
    elf_ei_class = ELFCLASSNONE;

  return elf_ei_class;
}

Elf64_Half
emit_ei_data (const Elf64_Ehdr *ehdr)
{
  Elf64_Half elf_ei_data = ehdr->e_ident[EI_DATA];
  if (elf_ei_data < ELFDATA2LSB || elf_ei_data > ELFDATA2MSB)
    elf_ei_data = ELFDATANONE;

  return elf_ei_data;
}

Elf64_Half
emit_ei_osabi (const Elf64_Ehdr *ehdr)
{
  Elf64_Half elf_ei_osabi = ehdr->e_ident[EI_OSABI];
  if (elf_ei_osabi >= ELFOSABI_SOLARIS && elf_ei_osabi <= ELFOSABI_OPENBSD)
    elf_ei_osabi -= 2;
  else if (elf_ei_osabi >= ELFOSABI_ARM_AEABI)
    elf_ei_osabi = (ARRAY_SIZE (elf_osabi_id) - 1);

  return elf_ei_osabi;
}

Elf64_Half
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

Elf64_Word
emit_e_version (const Elf64_Ehdr *ehdr)
{
  Elf64_Word elf_e_version
      = ehdr->e_version != EV_CURRENT ? EV_NONE : ehdr->e_version;

  return elf_e_version;
}

void
display_elf_header (const Elf64_Ehdr *ehdr)
{
  Elf64_Half elf_ei_class = emit_ei_class (ehdr);
  Elf64_Half elf_ei_data = emit_ei_data (ehdr);
  Elf64_Half elf_ei_osabi = emit_ei_osabi (ehdr);
  Elf64_Half elf_e_type = emit_e_type (ehdr);
  Elf64_Word elf_e_version = emit_e_version (ehdr);

  printf ("ELF Header:\n"
          "  Magic:   ");

  for (int i = 0; i < EI_NIDENT; i++)
    printf ("%.2x ", ehdr->e_ident[i]);

  putchar ('\n');
  printf ("  Class:                               %s\n"
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
          ehdr->e_version, elf_e_version_id[elf_e_version], (int)ehdr->e_entry,
          (int)ehdr->e_phoff, (int)ehdr->e_shoff, (int)ehdr->e_flags,
          (int)ehdr->e_ehsize, (int)ehdr->e_phentsize, (int)ehdr->e_phnum,
          (int)ehdr->e_shentsize, (int)ehdr->e_shnum, (int)ehdr->e_shstrndx);
}

int
read_file_into_mem (const char *filename, void **data_out, size_t *size_out)
{
  struct stat sb;
  FILE *file;
  long filesize;
  void *mem;
  int fd;

  file = fopen (filename, "rb");
  if (file == NULL)
    goto err_ret;

  fd = fileno (file);
  if ((fstat (fd, &sb) == -1) || S_ISDIR (sb.st_mode))
    goto err_ret;

  if (fseek (file, 0, SEEK_END) == -1)
    goto err_close;

  errno = 0;
  filesize = ftell (file);
  if (filesize == -1L || errno != 0)
    goto err_close;
  rewind (file);

  mem = malloc (filesize);
  if (mem == NULL)
    goto err_close;

  if (fread (mem, filesize, 1, file) != 1)
    goto err_free;

  fclose (file);

  *data_out = mem;
  *size_out = filesize;

  return 1;
err_free:
  free (mem);
err_close:
  fclose (file);
err_ret:
  return 0;
}

int
write_mem_to_file (const char *filename, const void *data, size_t size)
{
  struct stat sb;
  int fd;
  int success = 0;

  FILE *output_file = fopen (filename, "wb");
  if (output_file == NULL)
    {
      printf ("Failed to open %s for writing\n", filename);
      return 0;
    }

  if (fwrite (data, size, 1, output_file) != 1)
    {
      printf ("Failed to write data\n");
      goto err;
    }

  fd = fileno (output_file);

  if (fstat (fd, &sb))
    {
      printf ("Failed to stat %s\n", filename);
      goto err;
    }

  if (fchmod (fd, sb.st_mode | S_IXUSR | S_IXGRP | S_IXOTH))
    {
      printf ("Failed to fchmod %s\n", filename);
      goto err;
    }

  success = 1;

err:
  fclose (output_file);
  return success;
}

int
get_s_type_index (Elf64_Word type)
{
  int offs;
  switch (type)
    {
    case SHT_PROGBITS:
      offs = 1;
      break;
    case SHT_SYMTAB:
      offs = 2;
      break;
    case SHT_STRTAB:
      offs = 3;
      break;
    case SHT_RELA:
      offs = 4;
      break;
    case SHT_HASH:
      offs = 5;
      break;
    case SHT_DYNAMIC:
      offs = 6;
      break;
    case SHT_NOTE:
      offs = 7;
      break;
    case SHT_NOBITS:
      offs = 8;
      break;
    case SHT_REL:
      offs = 9;
      break;
    case SHT_SHLIB:
      offs = 10;
      break;
    case SHT_DYNSYM:
      offs = 11;
      break;
    case SHT_INIT_ARRAY:
      offs = 12;
      break;
    case SHT_FINI_ARRAY:
      offs = 13;
      break;
    case SHT_PREINIT_ARRAY:
      offs = 14;
      break;
    case SHT_GROUP:
      offs = 15;
      break;
    case SHT_SYMTAB_SHNDX:
      offs = 16;
      break;
    case SHT_NUM:
      offs = 17;
      break;
    case SHT_LOOS:
      offs = 18;
      break;
    case SHT_GNU_ATTRIBUTES:
      offs = 19;
      break;
    case SHT_GNU_HASH:
      offs = 20;
      break;
    case SHT_GNU_LIBLIST:
      offs = 21;
      break;
    case SHT_CHECKSUM:
      offs = 22;
      break;
    case SHT_LOSUNW:
      offs = 23;
      break;
    case SHT_SUNW_COMDAT:
      offs = 24;
      break;
    case SHT_SUNW_syminfo:
      offs = 25;
      break;
    case SHT_GNU_verdef:
      offs = 26;
      break;
    case SHT_GNU_verneed:
      offs = 27;
      break;
    case SHT_GNU_versym:
      offs = 28;
      break;
    case SHT_LOPROC:
      offs = 29;
      break;
    case SHT_HIPROC:
      offs = 30;
      break;
    case SHT_LOUSER:
      offs = 31;
      break;
    case SHT_HIUSER:
      offs = 32;
      break;
    default:
      offs = 0;
    }
  return offs;
}

int
get_p_type_index (Elf64_Word type)
{
  int offs;
  switch (type)
    {
    case PT_LOAD:
      offs = 1;
      break;
    case PT_DYNAMIC:
      offs = 2;
      break;
    case PT_INTERP:
      offs = 3;
      break;
    case PT_NOTE:
      offs = 4;
      break;
    case PT_SHLIB:
      offs = 5;
      break;
    case PT_PHDR:
      offs = 6;
      break;
    case PT_TLS:
      offs = 7;
      break;
    case PT_NUM:
      offs = 8;
      break;
    case PT_LOOS:
      offs = 9;
      break;
    case PT_GNU_EH_FRAME:
      offs = 10;
      break;
    case PT_GNU_STACK:
      offs = 11;
      break;
    case PT_GNU_RELRO:
      offs = 12;
      break;
    case PT_LOSUNW:
      offs = 13;
      break;
    case PT_SUNWSTACK:
      offs = 14;
      break;
    case PT_HISUNW:
      offs = 15;
      break;
    case PT_LOPROC:
      offs = 16;
      break;
    case PT_HIPROC:
      offs = 17;
      break;
    case PT_NULL:
    default:
      offs = 0;
      break;
    }
  return offs;
}

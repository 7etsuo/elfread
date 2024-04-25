#ifdef __APPLE__
#include <libelf/libelf.h>
#elif __linux__
#include <libelf.h>
#endif

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "./include/fileio.h"
#include "./include/my_elf.h"

char *
get_p_type (unsigned int p_type)
{
  switch (p_type)
    {
    case 0:
      return "NULL";
    case 1:
      return "LOAD";
    case 2:
      return "DYNAMIC";
    case 3:
      return "INTERP";
    case 4:
      return "NOTE";
    case 5:
      return "SHLIB";
    case 6:
      return "PHDR";
    case 7:
      return "TLS";
    case 8:
      return "NUM";
    case 0x60000000:
      return "LOOS";
    case 0x6474e550:
      return "GNU_EH_FRAME";
    case 0x6474e551:
      return "GNU_STACK";
    case 0x6474e552:
      return "GNU_RELRO";
    case 0x6474e553:
      return "GNU_PROPERTY";
    case 0x6474e554:
      return "GNU_SFRAME";
    case 0x6ffffffa:
      return "LOSUNW";
    case 0x6ffffffb:
      return "SUNWSTACK";
    case 0x6fffffff:
      return "HIOS";
    case 0x70000000:
      return "LOPROC";
    case 0x7fffffff:
      return "HIPROC";
    default:
      return "UNKNOWN";
    }
}

char *
get_p_flags (uint32_t p_flags, char *buf)
{
  if (buf == NULL)
    {
      fprintf (stderr, "buf is NULL.\n");
      return NULL;
    }

  (void)memset (buf, ' ', 3);

  if (p_flags & PF_X)
    buf[2] = 'X';
  if (p_flags & PF_W)
    buf[1] = 'W';
  if (p_flags & PF_R)
    buf[0] = 'R';

  return buf;
}

int
validate_elf_magic (const Elf64_Ehdr *ehdr)
{
  if (ehdr->e_ident[EI_MAG0] != ELFMAG0 || ehdr->e_ident[EI_MAG1] != ELFMAG1
      || ehdr->e_ident[EI_MAG2] != ELFMAG2
      || ehdr->e_ident[EI_MAG3] != ELFMAG3)
    {
      fprintf (stderr, "Invalid ELF magic number.\n");
      return -1;
    }

  return 0;
}

int
validate_elf_header (const Elf64_Ehdr *ehdr)
{
  if (validate_elf_magic (ehdr) != 0)
    {
      return -1;
    }

  if (ehdr->e_ident[EI_CLASS] != ELFCLASS64)
    {
      fprintf (stderr, "Invalid ELF class.\n");
      return -1;
    }

  if (ehdr->e_ident[EI_DATA] != ELFDATA2LSB)
    {
      fprintf (stderr, "Invalid ELF data encoding.\n");
      return -1;
    }

  if (ehdr->e_ident[EI_VERSION] != EV_CURRENT)
    {
      fprintf (stderr, "Invalid ELF version.\n");
      return -1;
    }

  return 0;
}

int
get_elf_header (void *buffer, size_t size, Elf64_Ehdr *ehdr)
{
  if (buffer == NULL)
    {
      fprintf (stderr, "Buffer is NULL.\n");
      return -1;
    }

  if (size < sizeof (Elf64_Ehdr))
    {
      fprintf (stderr, "Buffer is too small to store ELF header.\n");
      return -1;
    }

  if (memcpy (ehdr, buffer, sizeof (Elf64_Ehdr)) == NULL)
    {
      fprintf (stderr, "Failed to copy ELF header.\n");
      return -1;
    }

  if (validate_elf_header (ehdr) != 0)
    {
      return -1;
    }

  return 0;
}

int
get_elf_phdr (void *buffer, off_t offset, Elf64_Ehdr *ehdr, Elf64_Phdr *phdr)
{
  if (buffer == NULL)
    {
      fprintf (stderr, "Buffer is NULL.\n");
      return -1;
    }

  if (ehdr == NULL)
    {
      fprintf (stderr, "ELF header is NULL.\n");
      return -1;
    }

  if (phdr == NULL)
    {
      fprintf (stderr, "Program header is NULL.\n");
      return -1;
    }

  if (memcpy (phdr, (char *)buffer + ehdr->e_phoff + offset, ehdr->e_phentsize)
      == NULL)
    {
      fprintf (stderr, "Failed to copy program headers.\n");
      return -1;
    }

  return 0;
}

int
get_elf_shdr (void *buffer, off_t offset, Elf64_Ehdr *ehdr, Elf64_Shdr *shdr)
{
  if (buffer == NULL)
    {
      fprintf (stderr, "Buffer is NULL.\n");
      return -1;
    }

  if (ehdr == NULL)
    {
      fprintf (stderr, "ELF header is NULL.\n");
      return -1;
    }

  if (shdr == NULL)
    {
      fprintf (stderr, "Section header is NULL.\n");
      return -1;
    }

  if (memcpy (shdr, (char *)buffer + ehdr->e_shoff + offset, ehdr->e_shentsize * ehdr->e_shnum)
      == NULL)
    {
      fprintf (stderr, "Failed to copy section headers.\n");
      return -1;
    }

  return 0;
}

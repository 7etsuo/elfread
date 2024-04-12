#include <elf.h>
#include <stdio.h>
#include <string.h>

#include "my_elf.h"

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

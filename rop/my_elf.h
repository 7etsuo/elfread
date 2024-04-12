#ifndef MY_ELF_H
#define MY_ELF_H

#include <elf.h>
#include <stddef.h>

int get_elf_header (void *buffer, size_t size, Elf64_Ehdr *ehdr);
int validate_elf_magic (const Elf64_Ehdr *ehdr);
int validate_elf_header (const Elf64_Ehdr *ehdr);

#endif // MY_ELF_H

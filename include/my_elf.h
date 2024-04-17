#ifndef MY_ELF_H
#define MY_ELF_H

#if __APPLE__
#include <libelf/libelf.h>
#elif __linux__
#include <libelf.h>
#endif
#include "fileio.h"
#include <stddef.h>

int get_elf_phdr (void *buffer, off_t offset, Elf64_Ehdr *ehdr,
                  Elf64_Phdr *phdr);
int get_elf_header (void *buffer, size_t size, Elf64_Ehdr *ehdr);
int validate_elf_magic (const Elf64_Ehdr *ehdr);
int validate_elf_header (const Elf64_Ehdr *ehdr);
char *get_p_type (unsigned int p_type);
char *get_p_flags (uint32_t p_flags, char *buf);
int get_elf_shdr (void *buffer, off_t offset, Elf64_Ehdr *ehdr,
                  Elf64_Shdr *shdr);

#endif // MY_ELF_H

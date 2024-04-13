#ifndef MY_ELF_H
#define MY_ELF_H

#if __APPLE__
#include <libelf/libelf.h>
#elif __linux__
#include <libelf.h>
#endif
#include <stddef.h>
#include "fileio.h"

int get_elf_header(void *buffer, size_t size, Elf64_Ehdr *ehdr);
int validate_elf_magic(const Elf64_Ehdr *ehdr);
int validate_elf_header(const Elf64_Ehdr *ehdr);

#endif // MY_ELF_H

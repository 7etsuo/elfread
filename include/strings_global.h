#ifndef STRINGS_GLOBAL_H
#define STRINGS_GLOBAL_H

#ifdef __APPLE__
#include <libelf/libelf.h>
#elif __linux__
#include <libelf.h>
#endif

// in elfread.c
extern const char* elf_class_id[ELFCLASSNUM];
extern const char* elf_data_id[ELFDATANUM];
extern const char* elf_osabi_id[];
extern const char* elf_e_type_id[];
extern const char* elf_e_machine_id[EM_NUM];
extern const char* elf_e_version_id[EV_NUM];
extern const char* elf_p_type_id[]; 
extern const char* elf_s_type_id[]; 

#endif // STRINGS_GLOBAL_H;
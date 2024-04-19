#ifndef ELF_CONTROLLER_H
#define ELF_CONTROLLER_H

#define SIZE_TEMPBUF 1024

#define PHDR_MAIN_HEADER_TITLES_FORMAT "      %-14s %-18s %-18s %-18s\n"

#define PHDR_SUBHEADER_TITLES_FORMAT   "  %-18s %-18s %-18s %-6s %-6s\n", " "

int do_run_controller (const char *const filename);

#endif // ELF_CONTROLLER_H
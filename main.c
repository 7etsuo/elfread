#include <capstone/capstone.h>
#include <elf.h>

#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "./include/elf_menu.h"
#include "./include/fileio.h"
#include "./include/my_elf.h"

// ssize_t
// disassemble_code_section (const uint8_t *code, size_t size)
// {
//   csh handle;
//   if (cs_open (CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK)
//     return -1;
//
//   cs_insn *insn = NULL;
//   size_t count = cs_disasm (handle, code, 8, 0x1000, 0, &insn);
//   // "\x55\x48\x8b\x05\xb8\x13\x00\x00"
//
//   if (count > 0)
//     {
//       size_t j;
//       for (j = 0; j < count; j++)
//         {
//           printf ("0x%" PRIx64 ":\t%s\t\t%s\n", insn[j].address,
//                   insn[j].mnemonic, insn[j].op_str);
//         }
//       cs_free (insn, count);
//     }
//   cs_close (&handle);
//
//   return 1;
// }

int
main (int argc, char **argv)
{

  char *filename = "temp";
  if (argc == 2)
    {
      filename = argv[1];
    }

  FileContents *filecontents = robust_read_file (filename);
  if (filecontents == NULL)
    {
      return 1;
    }

  Elf64_Ehdr ehdr = { 0 };
  if (get_elf_header (filecontents->buffer, filecontents->length, &ehdr) != 0)
    {
      free (filecontents->buffer);
      free (filecontents);
      return 1;
    }

  do_elf_menu ();

  free (filecontents->buffer);
  free (filecontents);

  return 0;
}

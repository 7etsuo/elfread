
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "./include/elf_controller.h"

// #include <capstone/capstone.h>
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
app_runner (const char *const filename)
{
  return do_run_controller (filename);
}

int
main (int argc, char *argv[])
{
  char *filename = argc == 2 ? argv[1] : "testelf";

  return app_runner (filename);
}

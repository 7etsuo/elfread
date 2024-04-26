#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "./include/elf_controller.h"

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

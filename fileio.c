#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./include/fileio.h"

int
robust_fseek (FILE *stream, long offset, int whence)
{
  if (fseek (stream, offset, whence) != 0)
    {
      fprintf (stderr, "Failed to seek in file.\n");
      return -1;
    }

  return 0;
}

long
robust_ftell (FILE *stream)
{
  long position = ftell (stream);
  if (position == -1)
    {
      fprintf (stderr, "Failed to get file position.\n");
      return -1;
    }
  return position;
}

void *
robust_malloc (size_t size)
{
  void *memory = malloc (size);
  if (memory == NULL)
    fprintf (stderr, "Failed to allocate memory.\n");
  return memory;
}

void
robust_free (void *ptr)
{
  if (ptr == NULL)
    {
      fprintf (stderr, "Attempted to free NULL pointer.\n");
      return;
    }
  free (ptr);
}

FILE *
robust_fopen (const char *filename, const char *mode)
{
  if (strlen (filename) > PATH_MAX)
    {
      fprintf (stderr, "Filename is too long.\n");
      return NULL;
    }

  FILE *file = fopen (filename, mode);
  if (file == NULL)
    {
      fprintf (stderr, "Failed to open file.\n");
      return NULL;
    }
  return file;
}

int
robust_fclose (FILE *stream)
{
  if (fclose (stream) != 0)
    {
      fprintf (stderr, "Failed to close file.\n");
      return -1;
    }
  return 0;
}

size_t
robust_fread (void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  size_t read_length = fread (ptr, size, nmemb, stream);
  if (read_length != nmemb)
    {
      fprintf (stderr, "Failed to read file.\n");
      return -1;
    }
  return read_length;
}

FileContents *
allocate_buffer_for_file (FILE *file, FileContents *file_contents)
{
  if (robust_fseek (file, 0, SEEK_END) != 0)
    goto err;

  long length = robust_ftell (file);
  if (length == -1)
    goto err;

  if (robust_fseek (file, 0, SEEK_SET) != 0)
    goto err;

  file_contents->buffer = robust_malloc (length + 1);
  if (file_contents->buffer == NULL)
    goto err;

  file_contents->length = length;
  return file_contents;

err:
  if (file_contents->buffer != NULL)
    free (file_contents->buffer);
  return NULL;
}

FILE *
robust_fopen_secure (const char *filename, const char *mode)
{
  if (filename == NULL)
    {
      fprintf (stderr, "Filename is NULL.\n");
      return NULL;
    }

  if (mode == NULL)
    {
      fprintf (stderr, "Mode is NULL.\n");
      return NULL;
    }

  if (strpbrk (filename, "\\/:*?\"<>|") != NULL)
    {
      fprintf (stderr, "Filename contains invalid characters.\n");
      return NULL;
    }

  FILE *file = robust_fopen (filename, mode);

  return file;
}

FileContents *
robust_read_file (const char *filename)
{
  FILE *file = robust_fopen_secure (filename, "r");
  if (file == NULL)
    return NULL;

  FileContents *file_contents = robust_malloc (sizeof (FileContents));
  if (file_contents == NULL)
    goto err;

  if (allocate_buffer_for_file (file, file_contents) == NULL)
    {
      fprintf (stderr, "Failed to allocate buffer for file.\n");
      goto err;
    }

  size_t read_length
      = robust_fread (file_contents->buffer, 1, file_contents->length, file);
  if (read_length == -1)
    goto err;

  if (robust_fclose (file) != 0)
    goto err;

  return file_contents;

err:
  if (file_contents->buffer != NULL)
    free (file_contents->buffer);

  if (file_contents != NULL)
    free (file_contents);

  if (file != NULL)
    robust_fclose (file);

  return NULL;
}

#ifdef TEST_FILEIO
int
main (int argc, char **argv)
{
  if (argc != 2)
    {
      fprintf (stderr, "Usage: %s <filename>\n", argv[0]);
      return 1;
    }

  char *buffer = robust_read_file (argv[1]);
  if (buffer == NULL)
    return 2;

  printf ("%s\n", buffer);

  free (buffer);

  return 0;
}
#endif

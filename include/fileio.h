#ifndef FILEIO_H
#define FILEIO_H

#include <stdio.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif // PATH_MAX

typedef struct
{
  char *buffer;
  size_t length;
} FileContents;

int robust_fseek (FILE *stream, long offset, int whence);
long robust_ftell (FILE *stream);
void *robust_malloc (size_t size);
void robust_free (void *ptr);
FILE *robust_fopen (const char *filename, const char *mode);
int robust_fclose (FILE *stream);
size_t robust_fread (void *ptr, size_t size, size_t nmemb, FILE *stream);

FileContents *allocate_buffer_for_file (FILE *file,
                                        FileContents *file_contents);
FILE *robust_fopen_secure (const char *filename, const char *mode);
FileContents *robust_read_file (const char *filename);

#endif // ROBUSTFILEIO_H

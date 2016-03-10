/* Database file format.

Copyright (C) 2005 Red Hat, Inc. All rights reserved.

You can use this file without restriction.

This file is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.

(This file defines a file format, so even the LGPL seems too restrictive.
   Share and enjoy.)

Author: Miloslav Trmac <mitr@redhat.com> */

#ifndef DB_H__
#define DB_H__

#include <stdint.h>

/* See doc/mlocate.db.5.in */

/* File header */
struct db_header
{
  uint8_t magic[8];		/* See DB_MAGIC below */
  uint32_t conf_size;		/* Configuration block size, in big endian */
  uint8_t version;	       /* File format version, see DB_VERSION* below */
  uint8_t check_visibility;	/* Check file visibility in locate(1) */
  uint8_t pad[2];		/* 32-bit total alignment */
  /* Followed by NUL-terminated path of the root of the database */
};
/* Followed by a configuration block and an EOF-terminated sequence of
   directories.

   The configuration block is a sequence of name-values pairs
   (variable name '\0' (variable value '\0')... '\0'), ordered by name.  If
   more than one value of a variable is present, they are ordered as if by
   strcmp ().
   
   Directory records are not output for unreadable directories; such
   directories do have an entry in their parent directory.

   "/" does not have a parent directory, so it is not present in the database
   at all. */

/* Contains a '\0' byte to unambiguously mark the file as a binary file. */
#define DB_MAGIC { '\0', 'm', 'l', 'o', 'c', 'a', 't', 'e' }

#define DB_VERSION_0 0x00

/* Directory header */
struct db_directory
{
  /* Both values 0 mean "invalid, always reread".  This coincides with what
     Linux FAT, romfs and cramfs drivers do for unknown data.

     ctime should in theory be sufficient, but several Linux filesystems fill
     ctime with creation time.  Therefore "time" is actually "max of ctime,
     mtime". */
  uint64_t time_sec;	       /* st_[cm]time of the directory in big endian */
  /* st_[cm]tim.tv_nsec of the directory in big endian or 0 if not available */
  uint32_t time_nsec;
  uint8_t pad[4];		/* 64-bit total alignment */
  /* Followed by NUL-terminated absolute path of the directory */
};
/* Followed by directory entries terminated by DBE_END, sorted by name using
   strcmp () */

/* Directory entry */
struct db_entry
{
  uint8_t type;			/* See DBE_* below */
  /* Followed by NUL-terminated name if tag != DBE_END */
};

enum
  {
    DBE_NORMAL		= 0,	/* A non-directory file */
    DBE_DIRECTORY	= 1,	/* A directory */
    DBE_END		= 2   /* End of directory contents; contains no name */
  };

#endif

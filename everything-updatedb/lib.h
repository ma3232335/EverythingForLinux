/* Common functions.

Copyright (C) 2005, 2007 Red Hat, Inc. All rights reserved.
This copyrighted material is made available to anyone wishing to use, modify,
copy, or redistribute it subject to the terms and conditions of the GNU General
Public License v.2.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 51 Franklin
Street, Fifth Floor, Boston, MA 02110-1301, USA.

Author: Miloslav Trmac <mitr@redhat.com> */

#ifndef LIB_H__
#define LIB_H__

#include <config.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>

#include "gettext.h"
#include "obstack.h"

#include "db.h"

#ifdef __GNUC__
#define attribute__(X) __attribute__ (X)
#else
#define attribute__(X)
#endif

#define _(X) gettext(X)

/* Number of entries in ARRAY */
#define ARRAY_SIZE(array) (sizeof (array) / sizeof (*array))

/* Convert VAL to big endian */
extern uint64_t htonll (uint64_t val);

/* Convert VAL from big endian */
extern uint64_t ntohll (uint64_t val);

/* Initialize dir_path_cmp_table */
extern void dir_path_cmp_init (void);

/* Compare two path names using the database directory order. This is not
   exactly strcmp () order: "a" < "a.b", so "a/z" < "a.b". */
extern int dir_path_cmp (const char *a, const char *b);

/* Functions used by obstack code */
extern struct _obstack_chunk *obstack_chunk_alloc (long size);

#define obstack_chunk_free free

/* The obstack code is too portable to be perfect :(.  So we need some
   better substitutes to avoid 32-bit overflows. */
#define OBSTACK_OBJECT_SIZE(H) \
  (size_t)((char *)obstack_next_free (H) - (char *)obstack_base (H))

/* To avoid obstack integer overflows, limit large obstack allocations to this
   value. */
enum { OBSTACK_SIZE_MAX = 1024 * 1024 };

/* A list of strings */
struct string_list
{
  char **entries;
  size_t len;			/* Number of valid entries */
  /* Number of allocated entries, usually invalid after the list is "finished"
     and ENTRIES are reallocated to the exact size. */
  size_t allocated;
};

/* Append STRING to LIST */
extern void string_list_append (struct string_list *list, char *string);

/* Sort LIST using dir_path_cmp () */
extern void string_list_dir_path_sort (struct string_list *list);

/* Is PATH included in LIST?  Update *IDX to move within LIST.

   LIST is assumed to be sorted using dir_path_cmp (), successive calls to this
   function are assumed to use PATH values increasing in dir_path_cmp (). */
extern bool string_list_contains_dir_path (const struct string_list *list,
					   size_t *idx, const char *path);

/* An open database */
struct db
{
  int fd;
  const char *filename;
  off_t read_bytes;		/* Total bytes read */
  bool quiet;			/* Don't report read errors */
  int err;			/* errno on last read error or 0 */
  char *buf_pos, *buf_end;
  char buffer[BUFSIZ];
};

/* Open FILENAME (already open as FD), as DB, set DB's quiet flag to QUIET.
   Store database header to *HEADER;
   return 0 if OK, -1 on error.

   Takes ownership of FD: it will be closed by db_close () or before return
   from this function if it fails.

   FILENAME must stay valid until db_close (). */
extern int db_open (struct db *db, struct db_header *header, int fd,
		    const char *filename, bool quiet);

/* Close DB */
extern void db_close (struct db *db);

/* Report read error or unexpected EOF on DB if not DB->quiet */
extern void db_report_error (const struct db *db);

/* Read SIZE (!= 0) bytes from DB to BUF;
   return 0 if OK, -1 on error */
extern int db_read (struct db *db, void *buf, size_t size);

/* Read a NUL-terminated string from DB to current object in OBSTACK (without
   the terminating NUL), report error on failure if not DB->quiet.
   return 0 if OK, or -1 on I/O error. */
extern int db_read_name (struct db *db, struct obstack *h);

/* Skip SIZE bytes in DB, report error on failure if not DB->quiet;
   return 0 if OK, -1 on error */
extern int db_skip (struct db *db, off_t size);

/* Return number of bytes read from DB so far  */
extern off_t db_bytes_read (const struct db *db);

#endif

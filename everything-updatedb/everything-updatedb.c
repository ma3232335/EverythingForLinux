#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sqlite3.h>
#include <dirent.h>

#include "lib.h"

/* Contains a single, usually not obstack_finish ()'ed object */
static struct obstack path_obstack;

static struct obstack name_obstack;

static bool conf_quiet;

sqlite3 *g_sqlite3db;

#define mlocatedb_path "/var/lib/mlocate"
#define mlocatedb_name "mlocate.db"
#define sqlite3db_path "/var/lib/everything"
#define sqlite3db_name "everything.db"

#define TABLENAME "everything"

static char sql_insert[] = "INSERT INTO "TABLENAME" (NAME, PATH, TYPE) VALUES(?1, ?2, ?3);";
static char sql_create_index[] = "CREATE INDEX "TABLENAME"_idx ON "TABLENAME"(NAME);";
sqlite3_stmt *stmt;

/* Statistics of the current database */
static uintmax_t stats_directories;
static uintmax_t stats_entries;
static uintmax_t stats_bytes;

/* Clear current statistics */
static void
stats_clear (void)
{
  stats_directories = 0;
  stats_entries = 0;
  stats_bytes = 0;
}

int insertData(char *name, char *path, int type)
{
    /* insert data */
    sqlite3_bind_text(stmt, 1, name, strlen(name), SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, path, strlen(path), SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, type);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        fprintf(stderr, "Commit Failed!\n");
    }
    sqlite3_reset(stmt);
    return 0;
}

/* Read and handle a directory in DB with HEADER (read just past struct
   db_directory);
   return 0 if OK, -1 on error or reached conf_output_limit

   path_obstack may contain a partial object if this function returns -1. */
static int handle_directory (struct db *db, const struct db_header *hdr)
{
    size_t size, dir_name_len;
    int visible;
    void *path;
    void *name;

    stats_directories++;

    /* read the name of this directory */
    if (db_read_name (db, &path_obstack) != 0)
        goto err;
    size = OBSTACK_OBJECT_SIZE (&path_obstack);
    if (size == 0)
    {
        if (conf_quiet == false)
            error (0, 0, _("invalid empty directory name in `%s'"), db->filename);
        goto err;
    }
    obstack_1grow (&path_obstack, 0);
    path = obstack_finish (&path_obstack);

    dir_name_len = OBSTACK_OBJECT_SIZE (&path_obstack);
    visible = hdr->check_visibility ? -1 : 1;

    /* read all entrise in this directory */
    for (;;)
    {
        struct db_entry entry;

        /* read the type of entry, 0: normal file, 1: directory */
        if (db_read (db, &entry, sizeof (entry)) != 0)
        {
            db_report_error (db);
            goto err;
        }
        if (entry.type == DBE_END)
            break;
        
        /* read the name of this entry */
        if (db_read_name (db, &name_obstack) != 0)
            goto err;
        obstack_1grow(&name_obstack, 0);

        /* insert this entry info in sqlite3 database, include filename, path, type(file or not) */
        insertData(obstack_base(&name_obstack), path, entry.type);

        /* clear name_obstack */
        size = OBSTACK_OBJECT_SIZE (&name_obstack);
        if (size > OBSTACK_SIZE_MAX) /* No surprises, please */
        {
            if (conf_quiet == false)
                error (0, 0, _("file name length %zu in `%s' is too large"), size,
                        db->filename);
            goto err;
        }
        obstack_blank (&name_obstack, -(ssize_t)size);
    }
    name = obstack_finish(&name_obstack);
    obstack_free (&path_obstack, path);
    obstack_free (&name_obstack, name);
    return 0;

err:
    return -1;
}


static void handle_db (int fd, const char *database)
{
    struct db db;
    struct db_header hdr;
    struct db_directory dir;
    void *p;
    int visible;

    /* open locate db */
    if (db_open (&db, &hdr, fd, database, conf_quiet) != 0)
    {
        close(fd);
        goto err;
    }
    stats_clear ();

    /* read "/" */
    if (db_read_name (&db, &path_obstack) != 0)
        goto err_path;
    obstack_1grow (&path_obstack, 0);
    visible = hdr.check_visibility ? -1 : 1;
    p = obstack_finish (&path_obstack);
    obstack_free (&path_obstack, p);

    /* skip configure block */
    if (db_skip (&db, ntohl (hdr.conf_size)) != 0)
        goto err_path;

    /* transaction begin */
    fprintf(stdout, "Begin translation...\n");
    sqlite3_exec(g_sqlite3db, "BEGIN TRANSACTION", NULL, NULL, NULL);
    sqlite3_prepare_v2(g_sqlite3db, sql_insert, strlen(sql_insert), &stmt, NULL);

    /* read all directories */
    while (db_read (&db, &dir, sizeof (dir)) == 0)
    {
        /* read all  in this directory, and store them in sqlit3 db */
        if (handle_directory (&db, &hdr) != 0)
            goto err_path;
    }

    /* create index on table(NAME) */
    sqlite3_exec(g_sqlite3db, sql_create_index, NULL, NULL, NULL);

    /* transaction end */
    sqlite3_exec(g_sqlite3db, "COMMIT TRANSACTION", NULL, NULL, NULL);
    sqlite3_finalize(stmt);

    fprintf(stdout, "Translation completely\n");

    if (db.err != 0)
    {
        db_report_error (&db);
        goto err_path;
    }
    /* Fall through */
err_path:
    p = obstack_finish (&path_obstack);
err_free:
    obstack_free (&path_obstack, p);
    db_close (&db);
err:
    ;
}

static void handle_mlocatedb_path_entry (const char *entry)
{
    int fd;

    struct stat st;

    if (stat (entry, &st) != 0)
    {
        if (conf_quiet == false)
            error (0, errno, _("can not stat () `%s'"), entry);
        goto err;
    }
    fd = open (entry, O_RDONLY);
    if (fd == -1)
    {
        if (conf_quiet == false)
            error (0, errno, _("can not open `%s'"), entry);
        goto err;
    }
    if (fstat (fd, &st) != 0)
    {
        if (conf_quiet == false)
            error (0, errno, _("can not stat () `%s'"), entry);
        close (fd);
        goto err;
    }

    /* locate database is ok, handle it */
    handle_db (fd, entry); /* Closes fd */
err:
    ;
}

int disconnect_sqlite3db()
{
    sqlite3_close(g_sqlite3db);
}

int connect_sqlite3db()
{
    /* connect everything database */
    char *zErrMsg = 0;
    int rc;

    DIR *dir;
    if ((dir = opendir(sqlite3db_path)) == NULL)
    {
        if (mkdir(sqlite3db_path, 0755) == -1)
        {
            closedir(dir);
            return -1;
        }
    }
    closedir(dir);

    if (unlink(sqlite3db_path"/"sqlite3db_name) == -1)
    {
        fprintf(stderr, "Can't remove database: %s\n", sqlite3db_path"/"sqlite3db_name);
    }

    rc = sqlite3_open(sqlite3db_path"/"sqlite3db_name, &g_sqlite3db);
    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3db_path"/"sqlite3db_name);
        return -1;
    }
    else
    {
        fprintf(stdout, "Open database successfully\n");
    }

    /* create table */
    char *sql = "CREATE TABLE "TABLENAME"("
        "ID INTEGER PRIMARY KEY NOT NULL,"
        "NAME TEXT NOT NULL,"
        "PATH INTEGER NOT NULL,"
        "TYPE INTEGER NOT NULL);";
    rc = sqlite3_exec(g_sqlite3db, sql, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }
    else
    {
        fprintf(stdout, "Create table successfully\n");
    }

    return 0;
}

static void help()
{
    printf("Usage: everything-updatedb [OPTION]\n"
            "Update the database used by everything.\n"
            "\n"
            "-h         print this help\n"
            "\n"
            "Report bugs to mamingliang1990@163.com.\n"
            );
}

int main(int argc, char *argv[])
{
    if (argc == 2 && strcmp(argv[1] ,"-h") == 0)
    {
        help();
        exit(EXIT_SUCCESS);
    }

    if (connect_sqlite3db() != 0)
    {
        fprintf(stderr, "Connect everything database error!\n");
        exit(EXIT_FAILURE);
    }

    obstack_init(&path_obstack);
    obstack_init(&name_obstack);

    obstack_alignment_mask(&path_obstack) = 0;

    handle_mlocatedb_path_entry(mlocatedb_path"/"mlocatedb_name);

    disconnect_sqlite3db();
    exit(EXIT_SUCCESS);
}

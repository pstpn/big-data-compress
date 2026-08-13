# The SQLite file format

A database engine is easy to use and easy to treat as a black box. This study opens the box
by writing a reader and a writer for SQLite database files in C, going straight at the bytes
instead of linking the library, and then proving them right by having `sqlite3` read what
they wrote.

## The format

An SQLite database is one file of fixed-size pages, from 512 bytes to 64 KB. The first
hundred bytes are the header, carrying the `SQLite format 3` signature, the page size, the
version and the text encoding. Tables live in b-trees keyed by rowid, indexes in b-trees of
their own, and page 1 holds `sqlite_master`, the table that describes every other object in
the file.

Two encodings do the compressing. Integers are stored as varints of one to nine bytes, so a
small number costs a small number of bytes, and every row is a record of a header listing
the size and type of each column followed by the values themselves, with `NULL` taking no
space at all.

## What was written

The reader and the writer between them handle the file header and pages, encode and decode
varints, parse the record format for `NULL`, `INTEGER` and `TEXT`, and build and walk the
b-tree pages that hold a table.

## The check

The writer creates a database and fills a table, then the same rows are printed twice, once
by `sqlite3` and once by the reader written here. They agree, which is the only evidence
that matters for a format implementation.

```bash
make sqlite-read
make my-read
```

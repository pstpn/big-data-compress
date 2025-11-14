CC = gcc
CFLAGS = -O2 -Wall

all: sqlite-reader sqlite-writer

sqlite-reader: sqlite_reader.c
	$(CC) $(CFLAGS) -o sqlite_reader sqlite_reader.c

sqlite-writer: sqlite_writer.c
	$(CC) $(CFLAGS) -o sqlite_writer sqlite_writer.c

write: all
	./sqlite_writer mydb.sqlite mytable

sqlite-read: write
	sqlite3 mydb.sqlite "SELECT rowid, name FROM mytable;"

my-read: write
	./sqlite_reader mydb.sqlite mytable

clean:
	rm -f sqlite_reader sqlite_writer mydb.sqlite

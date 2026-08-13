# Data storage and compression

Four studies on how data is laid out on disk and what compressing it actually costs. Each
one takes a single question, builds whatever is needed to answer it, and settles it with
measurements rather than with theory.

| Study | Question |
|-------|----------|
| [Columnar formats against csv](lab_02) | What Parquet and ORC buy over plain csv, and which of the two is worth choosing |
| [The SQLite file format](lab_03) | How a database file is actually laid out, checked by reading and writing one without the library |
| [Huffman against zlib](lab_04) | What a hand-written entropy coder gives up to a dictionary method |
| [LOUDS](lab_05) | What a succinct tree saves in memory and what it costs in navigation |

Every study keeps its sources, its measurements and a full report with the plots in its own
directory.

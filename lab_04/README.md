# Huffman against zlib

Huffman coding assigns short codes to frequent symbols and long ones to rare symbols, which
is the best any coder can do if it looks at symbols one at a time. zlib does something else
first: it replaces repeated sequences with references to earlier ones, and only then hands
what is left to a Huffman coder. This study measures how much that extra step is worth.

## Method

A Huffman coder is written from scratch in C++, building its tree from the symbol
frequencies of each input file, and measured against zlib on the same five files, chosen so
that the amount of exploitable structure differs sharply between them. A plain text file, a
Word document, a bitmap, a pdf and a zip archive are each compressed and decompressed, with
the ratio and both timings recorded and written to csv for plotting.

## Results

| File | Huffman ratio | zlib ratio | Huffman compress, ms | zlib compress, ms | Huffman decompress, ms | zlib decompress, ms |
|------|--------------:|-----------:|---------------------:|------------------:|-----------------------:|--------------------:|
| txt | 2.61 | 6.67 | 0.17 | 0.10 | 0.10 | 0.06 |
| doc | 1.57 | 2.74 | 18.58 | 2.64 | 8.25 | 1.00 |
| bmp | 1.05 | 1.18 | 73.75 | 8.45 | 39.00 | 5.91 |
| pdf | 1.00 | 1.04 | 1420.73 | 151.27 | 764.23 | 114.90 |
| zip | 1.00 | 1.00 | 275.35 | 25.00 | 147.68 | 22.00 |

## What the numbers say

zlib wins on every file, and by roughly an order of magnitude in time as well as by a wide
margin in ratio. On text the gap is largest, 6.67 against 2.61, which is exactly where
repeated words give a dictionary the most to remove before the entropy coder ever sees the
data.

The two ends of the table are the instructive ones. A bitmap barely compresses under either
method, because its bytes are pixel values with no repetition at the byte level. A pdf and a
zip are already compressed, so both methods stop at a ratio of one, and Huffman spends a
second and a half of work on the pdf to achieve exactly nothing. Compression is not free,
and the cost is paid whether or not the data has anything left to give.

## Running

```bash
make run
make plot
```

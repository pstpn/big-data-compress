reset
set terminal pdfcairo enhanced font "Helvetica,11" size 8,6
set output "results/benchmark_comparison.pdf"

set datafile separator ","
set style data histograms
set style histogram clustered gap 2
set style fill solid border -1
set boxwidth 0.9
set grid ytics lw 1 lc rgb "#cccccc"
set key outside top center horizontal box

### Color palette
CSVColor       = "#1f77b4"
ParquetColor   = "#2ca02c"
ORCColor       = "#d62728"

### Input data file
datafile = "results/benchmark_results.csv"

### ============================================================
### 1. FILE SIZE COMPARISON
### ============================================================

set title "File Size Comparison: CSV, Apache Parquet, and Apache ORC" font ",13"
set xlabel "Dataset"
set ylabel "File Size (MB)"
set yrange [0:*]
set style fill solid 0.8 border -1

plot \
    datafile using 2:xtic(1) title "CSV"                     lc rgb CSVColor, \
    '' using 4 title "Parquet (Uncompressed)"                lc rgb ParquetColor, \
    '' using 6 title "Parquet (Snappy)"                      lc rgb ParquetColor dt 2, \
    '' using 8 title "Parquet (Gzip)"                        lc rgb ParquetColor dt 3, \
    '' using 10 title "Parquet (LZ4)"                        lc rgb ParquetColor dt 4, \
    '' using 12 title "Parquet (Zstd)"                       lc rgb ParquetColor dt 5, \
    '' using 14 title "ORC (None)"                           lc rgb ORCColor, \
    '' using 16 title "ORC (Snappy)"                         lc rgb ORCColor dt 2, \
    '' using 18 title "ORC (Zlib)"                           lc rgb ORCColor dt 3, \
    '' using 20 title "ORC (LZ4)"                            lc rgb ORCColor dt 4, \
    '' using 22 title "ORC (Zstd)"                           lc rgb ORCColor dt 5

### ============================================================
### 2. COMPRESSION RATE COMPARISON
### ============================================================

set title "Compression Rate Relative to CSV" font ",13"
set xlabel "Dataset"
set ylabel "Compression Rate (CSV / Format)"
set yrange [0:*]

plot \
    datafile using ($2/$2):xtic(1) title "CSV (Baseline)"          lc rgb CSVColor, \
    '' using ($2/$4)  title "Parquet (Uncompressed)"               lc rgb ParquetColor, \
    '' using ($2/$6)  title "Parquet (Snappy)"                     lc rgb ParquetColor dt 2, \
    '' using ($2/$8)  title "Parquet (Gzip)"                       lc rgb ParquetColor dt 3, \
    '' using ($2/$10) title "Parquet (LZ4)"                        lc rgb ParquetColor dt 4, \
    '' using ($2/$12) title "Parquet (Zstd)"                       lc rgb ParquetColor dt 5, \
    '' using ($2/$14) title "ORC (None)"                           lc rgb ORCColor, \
    '' using ($2/$16) title "ORC (Snappy)"                         lc rgb ORCColor dt 2, \
    '' using ($2/$18) title "ORC (Zlib)"                           lc rgb ORCColor dt 3, \
    '' using ($2/$20) title "ORC (LZ4)"                            lc rgb ORCColor dt 4, \
    '' using ($2/$22) title "ORC (Zstd)"                           lc rgb ORCColor dt 5

### ============================================================
### 3. READ TIME COMPARISON
### ============================================================

set title "Read Time Comparison: CSV, Apache Parquet, and Apache ORC" font ",13"
set xlabel "Dataset"
set ylabel "Read Time (seconds)"
set yrange [0:*]

plot \
    datafile using 3:xtic(1) title "CSV"                     lc rgb CSVColor, \
    '' using 5  title "Parquet (Uncompressed)"               lc rgb ParquetColor, \
    '' using 7  title "Parquet (Snappy)"                     lc rgb ParquetColor dt 2, \
    '' using 9  title "Parquet (Gzip)"                       lc rgb ParquetColor dt 3, \
    '' using 11 title "Parquet (LZ4)"                        lc rgb ParquetColor dt 4, \
    '' using 13 title "Parquet (Zstd)"                       lc rgb ParquetColor dt 5, \
    '' using 15 title "ORC (None)"                           lc rgb ORCColor, \
    '' using 17 title "ORC (Snappy)"                         lc rgb ORCColor dt 2, \
    '' using 19 title "ORC (Zlib)"                           lc rgb ORCColor dt 3, \
    '' using 21 title "ORC (LZ4)"                            lc rgb ORCColor dt 4, \
    '' using 23 title "ORC (Zstd)"                           lc rgb ORCColor dt 5

### ============================================================
### END OF REPORT
### ============================================================

unset output

#!/usr/bin/env gnuplot

set terminal pdfcairo enhanced font 'Arial,11' size 10,10
set output 'results/comparison.pdf'
set datafile separator ','
set style data histograms
set style histogram clustered gap 1
set style fill solid border -1
set boxwidth 0.8
set grid ytics lc rgb "#dddddd" lw 1
set key outside top center horizontal box

CSVColor     = "#FF6B6B"
ParquetColor = "#4ECDC4"
ORCColor     = "#45B7D1"

datafile = 'results/benchmark_results.csv'

set multiplot layout 2,2 title "csv vs parquet vs orc" font 'Arial,16' offset 0,-1

set title "File sizes" font 'Arial,14'
set xlabel "Datasets" font 'Arial,12'
set ylabel "Size (MB)" font 'Arial,12'
set yrange [0:*]
set key top left

plot datafile using 2:xtic(1) title 'csv'     lc rgb CSVColor, \
     '' using 3 title 'parquet' lc rgb ParquetColor, \
     '' using 4 title 'orc'     lc rgb ORCColor

set title "Compression ratio (relative to csv)" font 'Arial,14'
set xlabel "Datasets" font 'Arial,12'
set ylabel "Ratio" font 'Arial,12'
set yrange [0:*]
set key top right

plot datafile using ($2/$3):xtic(1) title 'parquet' lc rgb ParquetColor, \
     '' using ($2/$4) title 'orc' lc rgb ORCColor

set title "Read time" font 'Arial,14'
set xlabel "Datasets" font 'Arial,12'
set ylabel "Time (s)" font 'Arial,12'
set yrange [0:*]
set key top left

plot datafile using 5:xtic(1) title 'csv'     lc rgb CSVColor, \
     '' using 6 title 'parquet' lc rgb ParquetColor, \
     '' using 7 title 'orc'     lc rgb ORCColor

set title "Read speedup (relative to csv)" font 'Arial,14'
set xlabel "Datasets" font 'Arial,12'
set ylabel "Speedup (×)" font 'Arial,12'
set yrange [0:*]
set key top right

plot datafile using ($5/$6):xtic(1) title 'parquet' lc rgb ParquetColor, \
     '' using ($5/$7) title 'orc' lc rgb ORCColor

unset multiplot
unset output

#!/usr/bin/env gnuplot

set terminal pdf size 10,10 enhanced font 'Arial,12'
set output 'results/comparison.pdf'
set datafile separator ','
set style data histogram
set style histogram clustered
set style fill solid border -1
set boxwidth 0.8


set multiplot layout 2,2 title "CSV vs Parquet vs ORC" font 'Arial,16'

set title "File sizes" font 'Arial,14'
set ylabel "Size (MB)" font 'Arial,12'
set xlabel "Datasets" font 'Arial,12'
set grid ytics
set key top left
set yrange [0:*]

plot 'results/benchmark_results.csv' every ::1 using 2:xtic(1) title 'csv' with boxes lc rgb "#FF6B6B", \
     'results/benchmark_results.csv' every ::1 using 3:xtic(1) title 'parquet' with boxes lc rgb "#4ECDC4", \
     'results/benchmark_results.csv' every ::1 using 4:xtic(1) title 'orc' with boxes lc rgb "#45B7D1"

set title "Compression ratio" font 'Arial,14'
set ylabel "Compression ratio" font 'Arial,12'
set xlabel "Datasets" font 'Arial,12'
set grid ytics
set key top right

plot 'results/benchmark_results.csv' every ::1 using ($2/$3):xtic(1) title 'parquet' with boxes lc rgb "#4ECDC4", \
     'results/benchmark_results.csv' every ::1 using ($2/$4):xtic(1) title 'orc' with boxes lc rgb "#45B7D1"

set title "Read speed" font 'Arial,14'
set ylabel "Time (seconds)" font 'Arial,12'
set xlabel "Datasets" font 'Arial,12'
set grid ytics
set key top left
set yrange [0:*]

plot 'results/benchmark_results.csv' every ::1 using 5:xtic(1) title 'parquet' with boxes lc rgb "#4ECDC4", \
     'results/benchmark_results.csv' every ::1 using 6:xtic(1) title 'orc' with boxes lc rgb "#45B7D1"

unset multiplot

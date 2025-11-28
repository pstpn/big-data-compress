#!/usr/bin/env gnuplot

set terminal pdfcairo size 12,8 enhanced color font 'Arial,12'
set output 'all_plots.pdf'

set multiplot layout 2,2 title "Сравнение реализации алгоритма Хаффмана и zlib" font 'Arial,16'

set title "Коэффициент сжатия" font 'Arial,14'
set ylabel "Коэффициент сжатия" font 'Arial,12'
set xlabel "Тип файла" font 'Arial,12'
set style fill solid 0.7
set boxwidth 0.4
set style histogram clustered gap 1
set yrange [0:*]
set grid ytics
set border 3
set tics font ",10"
set key outside top center horizontal reverse

plot "< awk -F, 'BEGIN {print \"label huffman zlib\"} {if (\$1==\"Huffman\") h[\$2]=\$3; if (\$1==\"zlib\") z[\$2]=\$3} END {print \"txt\", h[\"data/file.txt\"], z[\"data/file.txt\"]; print \"pdf\", h[\"data/file.pdf\"], z[\"data/file.pdf\"]; print \"doc\", h[\"data/file.doc\"], z[\"data/file.doc\"]; print \"zip\", h[\"data/file.zip\"], z[\"data/file.zip\"]; print \"bmp\", h[\"data/file.bmp\"], z[\"data/file.bmp\"]}' out/results.csv" using 3:xtic(1) with boxes lc rgb "#2CA02C" title "zlib", '' using 2 with boxes lc rgb "#D62728" title "Huffman"

set title "Время кодирования" font 'Arial,14'
set ylabel "Время кодирования (мс)" font 'Arial,12'
set xlabel "Тип файла" font 'Arial,12'
set style fill solid 0.7
set boxwidth 0.4
set style histogram clustered gap 1
set yrange [0:*]
set grid ytics
set border 3
set tics font ",10"
set key outside top center horizontal

plot "< awk -F, 'BEGIN {print \"label huffman zlib\"} {if (\$1==\"Huffman\") h[\$2]=\$4; if (\$1==\"zlib\") z[\$2]=\$4} END {print \"txt\", h[\"data/file.txt\"], z[\"data/file.txt\"]; print \"pdf\", h[\"data/file.pdf\"], z[\"data/file.pdf\"]; print \"doc\", h[\"data/file.doc\"], z[\"data/file.doc\"]; print \"zip\", h[\"data/file.zip\"], z[\"data/file.zip\"]; print \"bmp\", h[\"data/file.bmp\"], z[\"data/file.bmp\"]}' out/results.csv" using 2:xtic(1) with boxes lc rgb "#D62728" title "Huffman", '' using 3 with boxes lc rgb "#2CA02C" title "zlib"

set title "Время декодирования" font 'Arial,14'
set ylabel "Время декодирования (мс)" font 'Arial,12'
set xlabel "Тип файла" font 'Arial,12'
set style fill solid 0.7
set boxwidth 0.4
set style histogram clustered gap 1
set yrange [0:*]
set grid ytics
set border 3
set tics font ",10"
set key outside top center horizontal

plot "< awk -F, 'BEGIN {print \"label huffman zlib\"} {if (\$1==\"Huffman\") h[\$2]=\$5; if (\$1==\"zlib\") z[\$2]=\$5} END {print \"txt\", h[\"data/file.txt\"], z[\"data/file.txt\"]; print \"pdf\", h[\"data/file.pdf\"], z[\"data/file.pdf\"]; print \"doc\", h[\"data/file.doc\"], z[\"data/file.doc\"]; print \"zip\", h[\"data/file.zip\"], z[\"data/file.zip\"]; print \"bmp\", h[\"data/file.bmp\"], z[\"data/file.bmp\"]}' out/results.csv" using 2:xtic(1) with boxes lc rgb "#D62728" title "Huffman", '' using 3 with boxes lc rgb "#2CA02C" title "zlib"

unset multiplot
unset output
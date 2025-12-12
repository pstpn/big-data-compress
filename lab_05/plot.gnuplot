set datafile separator ","

set terminal pdf size 11,14 enhanced font 'Arial,10'
set output 'comparison.pdf'

set multiplot layout 3,2 title "Сравнение обычного дерева и LOUDS дерева" font ",14"

set title 'Сравнение размера в памяти'
set xlabel 'Количество узлов'
set ylabel 'Размер (байты)'
set grid
set key left top
set logscale xy
plot 'benchmark_results.csv' using 1:2 with linespoints linewidth 2 pointtype 7 pointsize 1 title 'Обычное дерево', \
     'benchmark_results.csv' using 1:3 with linespoints linewidth 2 pointtype 9 pointsize 1 title 'LOUDS дерево'

set title 'FirstChild'
set xlabel 'Количество узлов'
set ylabel 'Время (наносекунды)'
set grid
set key left top
set logscale xy
plot 'benchmark_results.csv' using 1:4 with linespoints linewidth 2 pointtype 7 pointsize 1 title 'Обычное дерево', \
     'benchmark_results.csv' using 1:5 with linespoints linewidth 2 pointtype 9 pointsize 1 title 'LOUDS дерево'

set title 'LastChild'
set xlabel 'Количество узлов'
set ylabel 'Время (наносекунды)'
set grid
set key left top
set logscale xy
plot 'benchmark_results.csv' using 1:6 with linespoints linewidth 2 pointtype 7 pointsize 1 title 'Обычное дерево', \
     'benchmark_results.csv' using 1:7 with linespoints linewidth 2 pointtype 9 pointsize 1 title 'LOUDS дерево'

set title 'ChildrenCount'
set xlabel 'Количество узлов'
set ylabel 'Время (наносекунды)'
set grid
set key left top
set logscale xy
plot 'benchmark_results.csv' using 1:8 with linespoints linewidth 2 pointtype 7 pointsize 1 title 'Обычное дерево', \
     'benchmark_results.csv' using 1:9 with linespoints linewidth 2 pointtype 9 pointsize 1 title 'LOUDS дерево'

set title 'Parent'
set xlabel 'Количество узлов'
set ylabel 'Время (наносекунды)'
set grid
set key left top
set logscale xy
plot 'benchmark_results.csv' using 1:10 with linespoints linewidth 2 pointtype 7 pointsize 1 title 'Обычное дерево', \
     'benchmark_results.csv' using 1:11 with linespoints linewidth 2 pointtype 9 pointsize 1 title 'LOUDS дерево'

unset multiplot

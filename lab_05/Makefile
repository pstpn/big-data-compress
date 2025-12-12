all: run plot

run:
	go run cmd/main.go

plot:
	gnuplot plot.gnuplot

graph:
	dot -Tsvg -ograph.svg graph.gv

.PHONY: run
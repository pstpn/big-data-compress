package main

import (
	"os"

	"github.com/dominikbraun/graph"
	"github.com/dominikbraun/graph/draw"

	"succinct/pkg/louds"
	"succinct/pkg/tree"
)

func main() {
	t := tree.Generate(30)

	prefixTree := louds.FromTree(t)
	prefixTree.Print()

	g := graph.New(graph.StringHash, graph.Directed())
	tree.Export(t, g)

	file, err := os.Create("graph.gv")
	if err != nil {
		panic(err)
	}
	defer file.Close()

	err = draw.DOT(g, file)
	if err != nil {
		panic(err)
	}
}

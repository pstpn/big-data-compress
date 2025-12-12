package main

import (
	"encoding/csv"
	"fmt"
	"os"
	"time"

	"succinct/pkg/louds"
	"succinct/pkg/tree"

	"github.com/dominikbraun/graph"
	"github.com/dominikbraun/graph/draw"
)

type BenchmarkResult struct {
	NodesCount       int
	CommonTreeSize   int
	LoudsTreeSize    int
	CommonFirstChild time.Duration
	LoudsFirstChild  time.Duration
	CommonLastChild  time.Duration
	LoudsLastChild   time.Duration
	CommonChildCount time.Duration
	LoudsChildCount  time.Duration
	CommonParent     time.Duration
	LoudsParent      time.Duration
}

func runBenchmark(nodesCount int, iterations int) BenchmarkResult {
	commonTree := tree.Generate(nodesCount)
	loudsTree := louds.FromTree(commonTree)

	result := BenchmarkResult{
		NodesCount:     nodesCount,
		CommonTreeSize: commonTree.SizeBits() / 8,
		LoudsTreeSize:  loudsTree.SizeBits() / 8,
	}

	values := collectAllValues(commonTree)
	if len(values) == 0 {
		return result
	}

	start := time.Now()
	for i := 0; i < iterations; i++ {
		val := values[i%len(values)]
		commonTree.FirstChild(val)
	}
	result.CommonFirstChild = time.Since(start) / time.Duration(iterations)

	start = time.Now()
	for i := 0; i < iterations; i++ {
		val := values[i%len(values)]
		loudsTree.FirstChild(val)
	}
	result.LoudsFirstChild = time.Since(start) / time.Duration(iterations)

	start = time.Now()
	for i := 0; i < iterations; i++ {
		val := values[i%len(values)]
		commonTree.LastChild(val)
	}
	result.CommonLastChild = time.Since(start) / time.Duration(iterations)

	start = time.Now()
	for i := 0; i < iterations; i++ {
		val := values[i%len(values)]
		loudsTree.LastChild(val)
	}
	result.LoudsLastChild = time.Since(start) / time.Duration(iterations)

	start = time.Now()
	for i := 0; i < iterations; i++ {
		val := values[i%len(values)]
		commonTree.ChildrenCount(val)
	}
	result.CommonChildCount = time.Since(start) / time.Duration(iterations)

	start = time.Now()
	for i := 0; i < iterations; i++ {
		val := values[i%len(values)]
		loudsTree.ChildrenCount(val)
	}
	result.LoudsChildCount = time.Since(start) / time.Duration(iterations)

	start = time.Now()
	for i := 0; i < iterations; i++ {
		val := values[i%len(values)]
		commonTree.Parent(val)
	}
	result.CommonParent = time.Since(start) / time.Duration(iterations)

	start = time.Now()
	for i := 0; i < iterations; i++ {
		val := values[i%len(values)]
		loudsTree.Parent(val)
	}
	result.LoudsParent = time.Since(start) / time.Duration(iterations)

	return result
}

func collectAllValues(node *tree.Node) []string {
	values := []string{}
	if node.Value() != "" {
		values = append(values, node.Value())
	}
	for _, child := range node.Children() {
		values = append(values, collectAllValues(child)...)
	}
	return values
}

func main() {
	sizes := []int{10, 50, 100, 500, 1000, 5000, 10000}
	iterations := 10000

	var results []BenchmarkResult

	fmt.Println("Running benchmarks...")
	for _, size := range sizes {
		fmt.Printf("Testing with %d nodes...\n", size)
		result := runBenchmark(size, iterations)
		results = append(results, result)
	}

	file, err := os.Create("benchmark_results.csv")
	if err != nil {
		panic(err)
	}
	defer file.Close()

	writer := csv.NewWriter(file)
	defer writer.Flush()

	header := []string{
		"Nodes",
		"CommonTree Size (bytes)",
		"LoudsTree Size (bytes)",
		"CommonTree FirstChild (ns)",
		"LoudsTree FirstChild (ns)",
		"CommonTree LastChild (ns)",
		"LoudsTree LastChild (ns)",
		"CommonTree ChildrenCount (ns)",
		"LoudsTree ChildrenCount (ns)",
		"CommonTree Parent (ns)",
		"LoudsTree Parent (ns)",
	}
	writer.Write(header)

	for _, r := range results {
		row := []string{
			fmt.Sprintf("%d", r.NodesCount),
			fmt.Sprintf("%d", r.CommonTreeSize),
			fmt.Sprintf("%d", r.LoudsTreeSize),
			fmt.Sprintf("%d", r.CommonFirstChild.Nanoseconds()),
			fmt.Sprintf("%d", r.LoudsFirstChild.Nanoseconds()),
			fmt.Sprintf("%d", r.CommonLastChild.Nanoseconds()),
			fmt.Sprintf("%d", r.LoudsLastChild.Nanoseconds()),
			fmt.Sprintf("%d", r.CommonChildCount.Nanoseconds()),
			fmt.Sprintf("%d", r.LoudsChildCount.Nanoseconds()),
			fmt.Sprintf("%d", r.CommonParent.Nanoseconds()),
			fmt.Sprintf("%d", r.LoudsParent.Nanoseconds()),
		}
		writer.Write(row)
	}
}

func exportGraph(commonTree *tree.Node) {
	g := graph.New(graph.StringHash, graph.Directed())
	tree.Export(commonTree, g)

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

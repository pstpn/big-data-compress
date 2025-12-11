package tree

import (
	"math/rand"
	"strings"

	"github.com/dominikbraun/graph"
)

const alphabet = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz"

type Default struct {
	value    string
	parent   *Default
	children []*Default
}

func (d *Default) Value() string {
	return d.value
}

func (d *Default) Children() []*Default {
	return d.children
}

func Generate(nodesCount int) *Default {
	root := &Default{}

	initNodesCount := 1
	generate(root, &initNodesCount, nodesCount)

	return root
}

func Export(root *Default, g graph.Graph[string, string]) {
	err := g.AddVertex(root.value)
	if err != nil {
		panic(err)
	}

	for _, child := range root.children {
		Export(child, g)
	}

	if root.parent != nil {
		err = g.AddEdge(root.parent.value, root.value)
		if err != nil {
			panic(err)
		}
	}
}

func generate(root *Default, currentNodesCount *int, targetNodesCount int) {
	if *currentNodesCount >= targetNodesCount {
		return
	}

	root.children = append(root.children, &Default{
		value:    generateString(rand.Intn(7) + 7),
		parent:   root,
		children: nil,
	})
	*currentNodesCount++

	if rand.Intn(2) == 1 {
		generate(root.children[rand.Intn(len(root.children))], currentNodesCount, targetNodesCount)
	}

	for root.parent == nil && *currentNodesCount < targetNodesCount {
		generate(root.children[rand.Intn(len(root.children))], currentNodesCount, targetNodesCount)
	}
}

func generateString(length int) string {
	s := &strings.Builder{}
	for range length {
		s.WriteByte(alphabet[rand.Intn(len(alphabet))])
	}
	return s.String()
}

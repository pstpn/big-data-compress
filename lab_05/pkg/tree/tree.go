package tree

import (
	"fmt"
	"math/rand"
	"strings"

	"github.com/dominikbraun/graph"
)

const alphabet = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz"

// Node represents a node in a classic n-ary tree.
type Node struct {
	value    string  // Node's value
	parent   *Node   // Pointer to parent node
	children []*Node // Slice of child nodes
}

// Value returns the node's value.
func (n *Node) Value() string {
	return n.value
}

// Children returns the slice of child nodes.
func (n *Node) Children() []*Node {
	return n.children
}

// SizeBits calculates the total size of the tree in bits.
func (n *Node) SizeBits() int {
	s := 0
	sizeBits(n, &s)
	return s
}

// FirstChild returns the value of the first child of the node with the given parent value.
// Time complexity: O(n) due to findNode.
func (n *Node) FirstChild(parent string) string {
	node := n.findNode(parent)
	if node == nil || len(node.children) == 0 {
		return ""
	}
	return node.children[0].value
}

// LastChild returns the value of the last child of the node with the given parent value.
// Time complexity: O(n) due to findNode.
func (n *Node) LastChild(parent string) string {
	node := n.findNode(parent)
	if node == nil || len(node.children) == 0 {
		return ""
	}
	return node.children[len(node.children)-1].value
}

// ChildrenCount returns the number of children for the node with the given parent value.
// Time complexity: O(n) due to findNode.
func (n *Node) ChildrenCount(parent string) int {
	node := n.findNode(parent)
	if node == nil {
		return 0
	}
	return len(node.children)
}

// Parent returns the parent value of the node with the given child value.
// Time complexity: O(n) due to findNode.
func (n *Node) Parent(child string) string {
	node := n.findNode(child)
	if node == nil || node.parent == nil {
		return ""
	}
	return node.parent.value
}

// Generate creates a random tree with the specified number of nodes.
func Generate(nodesCount int) *Node {
	root := &Node{}

	initNodesCount := 1
	generate(root, &initNodesCount, nodesCount)

	return root
}

// Export exports the tree to a graph structure for visualization.
func Export(root *Node, g graph.Graph[string, string]) {
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

func (n *Node) Print() {
	fmt.Println()
	index := 0
	n.printNode(&index)
}

func (n *Node) printNode(index *int) {
	currentIndex := *index
	v := n.value
	fmt.Printf("%d) %q\n    |\n    -- "+
		"parent: %q\n       "+
		"first child: %q\n       "+
		"last child: %q\n       "+
		"children count: %d\n\n",
		currentIndex,
		v,
		n.Parent(v),
		n.FirstChild(v),
		n.LastChild(v),
		n.ChildrenCount(v),
	)
	*index++
	for _, child := range n.children {
		child.printNode(index)
	}
}

// findNode recursively searches for a node with the given value.
// Time complexity: O(n) - performs depth-first search.
func (n *Node) findNode(value string) *Node {
	if n.value == value {
		return n
	}
	for _, child := range n.children {
		if found := child.findNode(value); found != nil {
			return found
		}
	}
	return nil
}

func sizeBits(root *Node, s *int) {
	for _, child := range root.children {
		sizeBits(child, s)
	}
	// (size of value + ptr to parent + ptrs to children)
	*s += (len(root.value) + 8 + len(root.children)*8) * 8
}

func generate(root *Node, currentNodesCount *int, targetNodesCount int) {
	if *currentNodesCount >= targetNodesCount {
		return
	}

	root.children = append(root.children, &Node{
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

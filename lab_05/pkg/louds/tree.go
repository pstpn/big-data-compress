package louds

import (
	"fmt"
	"strings"

	"succinct/pkg/tree"
)

// Tree represents a LOUDS (Level-Order Unary Degree Sequence) encoded tree.
// It uses a bitmap representation for space-efficient tree storage.
type Tree struct {
	lbs           FakeBitmap         // Level-order bitmap sequence
	index         map[int]*nodeIndex // Maps node index to its data
	invertedIndex map[string]int     // Maps node value to its index (for O(1) lookup)
}

type nodeIndex struct {
	value    string // Node value
	children int    // Number of children (cached for performance)
}

// FirstChild returns the value of the first child of the given parent node.
// Returns empty string if parent has no children or doesn't exist.
func (t *Tree) FirstChild(parent string) string {
	parentIndex := t.invertedIndex[parent]
	if t.index[parentIndex].children < 1 {
		return ""
	}

	return t.index[t.FirstChildIndex(parentIndex)].value
}

// FirstChildIndex returns the index of the first child for the given parent index.
func (t *Tree) FirstChildIndex(parentIndex int) int {
	return t.lbs.Select(parentIndex+1, 0) - parentIndex
}

// LastChild returns the value of the last child of the given parent node.
// Returns empty string if parent has no children or doesn't exist.
func (t *Tree) LastChild(parent string) string {
	lastChildIndex := t.LastChildIndex(t.invertedIndex[parent])
	if lastChildIndex < 0 {
		return ""
	}

	return t.index[lastChildIndex].value
}

func (t *Tree) LastChildIndex(parentIndex int) int {
	lastChildIndex := t.lbs.Select(parentIndex+2, 0) - 1
	if t.lbs[lastChildIndex] != 1 {
		return -1
	}

	return t.lbs.Rank(lastChildIndex-1, 1)
}

// ChildrenCount returns the number of children for the given parent node.
func (t *Tree) ChildrenCount(parent string) int {
	parentIndex := t.invertedIndex[parent]
	count := t.LastChildIndex(parentIndex) - t.FirstChildIndex(parentIndex) + 1
	if count < 0 {
		return 0
	}

	return count
}

// Parent returns the value of the parent node for the given child.
// Returns empty string if child is root or doesn't exist.
func (t *Tree) Parent(child string) string {
	childIndex := t.invertedIndex[child]
	parent := t.index[t.lbs.Rank(t.lbs.Select(childIndex+1, 1)-1, 0)-1]
	if parent == nil {
		return ""
	}

	return parent.value
}

// SizeBits returns the total size of the tree in bits.
func (t *Tree) SizeBits() int {
	// Size of lbs bitmap
	s := len(t.lbs)
	for _, v := range t.index {
		// (size of int key + size of value)
		s += (4 + len(v.value)) * 8
	}
	return s
}

func (t *Tree) Print() {
	fmt.Println()
	for i := range len(t.index) {
		v := t.index[i].value
		fmt.Printf("%d) %q\n    |\n    -- "+
			"parent: %q\n       "+
			"first child: %q\n       "+
			"last child: %q\n       "+
			"children count: %d\n\n",
			i,
			v,
			t.Parent(v),
			t.FirstChild(v),
			t.LastChild(v),
			t.ChildrenCount(v),
		)
	}
	fmt.Println(t.lbs)
	fmt.Println()
}

// FromTree converts a regular tree to LOUDS representation.
func FromTree(root *tree.Node) *Tree {
	pt := &Tree{
		lbs:           append(NewBitmap(), []byte{1, 0}...),
		index:         make(map[int]*nodeIndex),
		invertedIndex: make(map[string]int),
	}
	queue := []*tree.Node{root}

	currentIndex := 0
	for len(queue) > 0 {
		current := queue[0]
		queue = queue[1:]

		pt.lbs = append(pt.lbs, generateNodeSeq(current)...)
		pt.index[currentIndex] = &nodeIndex{
			value:    current.Value(),
			children: len(current.Children()),
		}
		pt.invertedIndex[current.Value()] = currentIndex
		currentIndex++

		queue = append(queue, current.Children()...)
	}

	return pt
}

func generateNodeSeq(node *tree.Node) []byte {
	seq := &strings.Builder{}
	for range node.Children() {
		seq.WriteByte(1)
	}
	seq.WriteByte(0)
	return []byte(seq.String())
}

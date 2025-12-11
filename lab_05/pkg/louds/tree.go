package louds

import (
	"fmt"
	"strings"

	"succinct/pkg/tree"
)

type PrefixTree struct {
	lbs           FakeBitmap
	index         map[int]*nodeIndex
	invertedIndex map[string]int
}

type nodeIndex struct {
	value    string
	children int
}

func (p *PrefixTree) FirstChild(parent string) string {
	parentIndex := p.invertedIndex[parent]
	if p.index[parentIndex].children < 1 {
		return ""
	}

	return p.index[p.FirstChildIndex(parentIndex)].value
}

func (p *PrefixTree) FirstChildIndex(parentIndex int) int {
	return p.lbs.Select(parentIndex+1, 0) - parentIndex
}

func (p *PrefixTree) LastChild(parent string) string {
	lastChildIndex := p.LastChildIndex(p.invertedIndex[parent])
	if lastChildIndex < 0 {
		return ""
	}

	return p.index[lastChildIndex].value
}

func (p *PrefixTree) LastChildIndex(parentIndex int) int {
	lastChildIndex := p.lbs.Select(parentIndex+2, 0) - 1
	if p.lbs[lastChildIndex] != 1 {
		return -1
	}

	return p.lbs.Rank(lastChildIndex-1, 1)
}

func (p *PrefixTree) ChildrenCount(parent string) int {
	parentIndex := p.invertedIndex[parent]
	count := p.LastChildIndex(parentIndex) - p.FirstChildIndex(parentIndex) + 1
	if count < 0 {
		return 0
	}

	return count
}

func (p *PrefixTree) Parent(child string) string {
	childIndex := p.invertedIndex[child]
	parent := p.index[p.lbs.Rank(p.lbs.Select(childIndex+1, 1)-1, 0)-1]
	if parent == nil {
		return ""
	}

	return parent.value
}

func (p *PrefixTree) Print() {
	fmt.Println()
	for i := range len(p.index) {
		v := p.index[i].value
		fmt.Printf("%d) %q\n    |\n    -- "+
			"parent: %q\n       "+
			"first child: %q\n       "+
			"last child: %q\n       "+
			"children count: %d\n\n",
			i,
			v,
			p.Parent(v),
			p.FirstChild(v),
			p.LastChild(v),
			p.ChildrenCount(v),
		)
	}
	fmt.Println(p.lbs)
	fmt.Println()
}

func FromTree(root *tree.Default) *PrefixTree {
	pt := &PrefixTree{
		lbs:           append(NewBitmap(), []byte{1, 0}...),
		index:         make(map[int]*nodeIndex),
		invertedIndex: make(map[string]int),
	}
	queue := []*tree.Default{root}

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

func generateNodeSeq(node *tree.Default) []byte {
	seq := &strings.Builder{}
	for range node.Children() {
		seq.WriteByte(1)
	}
	seq.WriteByte(0)
	return []byte(seq.String())
}

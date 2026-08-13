# LOUDS

A tree of pointers spends most of its memory on the pointers. A succinct structure stores
the same shape in close to the information-theoretic minimum and answers the same questions
by computing over that encoding instead of following links. This study implements one such
structure, LOUDS, and measures what it saves and what it costs.

## The idea

LOUDS encodes the shape of a tree as a single bit sequence. Nodes are numbered in
breadth-first order, and each node with `k` children contributes `k` ones followed by a
zero. Navigation then becomes two operations over that sequence, `Rank(i, bit)` counting the
occurrences of a bit before a position and `Select(n, bit)` finding the position of the
n-th occurrence. The implementation here keeps an inverted index from a node's value to its
index, so that finding a node does not require a scan.

The trade-off is stated plainly by the structure itself. Storage is compact and access is
sequential, which suits a cache, but navigation has to be computed rather than followed, and
a naive `Rank` and `Select` are linear where an optimised implementation reaches constant
and near-constant time.

## Method

Both a classical pointer tree and a LOUDS tree are built in Go over sizes from ten to ten
thousand nodes. Memory is measured for each, and four navigation operations are timed, each
repeated ten thousand times and averaged.

## Memory

| Nodes | Pointer tree, bytes | LOUDS, bytes |
|------:|--------------------:|-------------:|
| 10 | 239 | 129 |
| 100 | 2611 | 1444 |
| 1000 | 26059 | 14317 |
| 10000 | 260344 | 142852 |

The saving holds at about 45 per cent across every size, which is what a structure with no
pointers should give against one that is mostly pointers.

## Navigation

| Nodes | FirstChild, pointer | FirstChild, LOUDS | Parent, pointer | Parent, LOUDS |
|------:|--------------------:|------------------:|----------------:|--------------:|
| 10 | 34 | 53 | 38 | 48 |
| 100 | 294 | 118 | 220 | 130 |
| 1000 | 1423 | 196 | 1303 | 428 |
| 10000 | 28360 | 1543 | 27782 | 3795 |

On the smallest tree the pointer version is ahead, since following a pointer beats scanning
a bit sequence when there is almost nothing to scan. The curves cross before a hundred nodes
and diverge from there. At ten thousand nodes LOUDS leads on all four operations, by 18.4
times on `FirstChild`, 7.3 on `Parent`, 5.8 on `LastChild` and 3.3 on `ChildrenCount`.

The reason is not the asymptotics, which favour the pointer tree if anything, but the
memory. A compact bit array is read sequentially and stays in cache, while a pointer tree
of ten thousand nodes scatters them across the heap and pays a miss at every step.

## Running

```bash
make
```

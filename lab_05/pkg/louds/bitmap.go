package louds

type FakeBitmap []byte

func NewBitmap() FakeBitmap {
	return make(FakeBitmap, 0)
}

func (f FakeBitmap) Select(n int, target byte) int {
	count := 0

	for k := 0; k < len(f); k++ {
		if f[k] == target {
			count++
		}
		if count == n {
			return k
		}
	}

	return -1
}

func (f FakeBitmap) Rank(i int, target byte) int {
	rank := 0

	for k := 0; k <= i; k++ {
		if f[k] == target {
			rank++
		}
	}

	return rank
}

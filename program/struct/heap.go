package main

import "fmt"

type HeapNode interface {
	Less(other HeapNode) bool
}

type Heap struct {
	root []HeapNode
	size int
}

func (h *Heap) swap(i int, j int) {
	tmp := h.root[i]
	h.root[i] = h.root[j]
	h.root[j] = tmp
}

func (h *Heap) min(p int, c1 int, c2 int) int {
	min := p
	minkey := h.root[min]
	l := len(h.root)
	if (c1 < l) {
		c1key := h.root[c1]
		if !minkey.Less(c1key) {
			min = c1
			minkey = h.root[min]
		}
	}
	
	if (c2 < l) {
		c2key := h.root[c2]
		if !minkey.Less(c2key) {
			min = c2
		}
	}

	return min
}

func (h *Heap) up(idx int) {
	for {
		p := (idx - 1) / 2
		if idx == p || !h.root[idx].Less(h.root[p]) {
			break
		}
		h.swap(p, idx)
		idx = p
	}
}

func (h *Heap) down(p int, len int) {
	for {
		min := p
		c1 := 2*p + 1;
		if (c1 >= len ) {
			break
		}

		if h.root[c1].Less(h.root[p]) {
			min = c1
		}

		c2 := c1 + 1;
		if c2 < len && h.root[c2].Less(h.root[min]) {
			min = c2
		}

		if min == p {
			break
		}
		h.swap(p, min)
		p = min
	}
}

func (h *Heap) Push(node HeapNode) {
	h.root = append(h.root, node)
	h.up(len(h.root) - 1)
}

func (h *Heap) Pop() HeapNode {
	l := len(h.root)
	h.swap(0, l - 1)
	ret := h.root[l - 1]
	h.root = h.root[0: l - 1]
	h.down(0, l - 1)
	return ret
}

func (h *Heap) Len() int {
	return len(h.root)
}

func (h *Heap)Dump() {
	for i := range h.root {
		node := h.root[i]
		fmt.Println(node)
	}
}

type IntNode int 

func (one IntNode) Less(other HeapNode) bool {
	r := one - other.(IntNode)
	return r < 0
}
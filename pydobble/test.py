#!/bin/python3

import dobble
import itertools
import sys

def test_generator(q: int) -> bool:
    d = dobble.DobbleDeck(q)

    assert(len(d) == q**2+q+1)

    cards = d.cards

    for c in cards:
        assert(len(c) == q+1)

    for c1, c2 in itertools.combinations(cards, 2):
        n_same = 0
        for n in c1:
            n_same += n in c2
        assert(n_same == 1)

    return True

if __name__ == "__main__":
    qs = [2, 3, 5, 7, 11, 13, 17, 23, 29, 31]
    for q in qs:
        test_generator(q)
        print("generator passed for q =", q)
    print("passed all generation tests")

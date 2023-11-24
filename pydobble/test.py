#!/bin/python3

import dobble
import itertools
import sys
import timeit

def test_generator(q: int) -> bool:
    start = timeit.default_timer()
    d = dobble.DobbleDeck(q)
    delta_t = timeit.default_timer() - start

    assert(len(d) == q**2+q+1)

    cards = d.cards

    for c in cards:
        assert(len(c) == q+1)

    for c1, c2 in itertools.combinations(cards, 2):
        n_same = 0
        for n in c1:
            n_same += n in c2
        assert(n_same == 1)

    return delta_t

if __name__ == "__main__":
    qs = [2, 3, 4, 5, 7, 8, 9, 11, 13, 16, 17, 23, 27, 29, 31, 32]
    for q in qs:
        try:
            delta_t = test_generator(q)
        except KeyboardInterrupt:
            print("keyboard interrupt")
            exit(130)
        print(f"passed for q = {q} (generated in {round(delta_t, 4)}s)")
    print("passed all generation tests")

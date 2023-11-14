#!/bin/python3

import dobble
import itertools

def test_generator(q: int) -> bool:
    d = dobble.generate(q)

    assert(len(d) == q**2+q+1)

    for c in d:
        assert(len(c) == q+1)

    for c1, c2 in itertools.combinations(d, 2):
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
    print("passed all prime tests")
    
    qs = [4, 8, 9, 16]
    for q in qs:
        test_generator(q)
        print("generator passed for q =", q)
    print("passed all powers of primes tests")

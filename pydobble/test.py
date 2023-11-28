#!/bin/python3

import argparse
import dobble
import itertools
import timeit

def test_generator(q, cdobble=False):
    start = timeit.default_timer()
    d = dobble.DobbleDeck(q, cdobble=cdobble)
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
    parser = argparse.ArgumentParser()
    parser.add_argument("--cdobble", action="store_true", help="use C backend rather than Python (+Sympy)")
    parser.add_argument("--primes", action="store_true", help="only test prime qs")
    parser.add_argument("--prime-powers", action="store_true", help="only test prime power qs")
    args = parser.parse_args()

    primes = [2, 3, 5, 7, 11, 13, 17, 23, 29, 31]
    prime_powers = [4, 8, 9, 16, 27, 32]

    qs = []

    if args.primes and args.prime_powers:
        qs = primes + prime_powers
    else:
        if args.primes:
            qs += primes
        if args.prime_powers:
            qs += prime_powers
    qs.sort()

    for q in qs:
        try:
            delta_t = test_generator(q, cdobble=args.cdobble)
        except KeyboardInterrupt:
            print("keyboard interrupt")
            exit(130)
        print(f"passed for q = {q} (generated in {round(delta_t, 4)}s)")
    print("passed all generation tests")

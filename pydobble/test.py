#!/bin/python3

import argparse
import dobble
import itertools
import timeit

def test_generator(q, cdobble=False):
    print(f"q = {q}: ", end="", flush=True)

    start = timeit.default_timer()
    d = dobble.DobbleDeck(q, cdobble=cdobble)
    delta_t = timeit.default_timer() - start

    print(f"generated in {round(delta_t, 4)} seconds... ", end="", flush=True)

    assert(len(d) == q**2+q+1)

    cards = d.cards

    for c in cards:
        assert(len(c) == q+1)

    for c1, c2 in itertools.combinations(cards, 2):
        n_same = 0
        for n in c1:
            n_same += n in c2
        assert(n_same == 1)

    print(f" passed checks")

    return

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--cdobble", action="store_true", help="use C backend rather than Python (+Sympy)")
    parser.add_argument("--primes", action="store_true", help="only test prime qs")
    parser.add_argument("--prime-powers", action="store_true", help="only test prime power qs")
    parser.add_argument("--single-q", type=int, help="only test prime power qs")
    args = parser.parse_args()

    primes = [2, 3, 5, 7, 11, 13, 17, 23, 29, 31]
    prime_powers = [4, 8, 9, 16, 27, 32]

    qs = primes+prime_powers

    if args.single_q:
        qs = [args.single_q]
    elif (args.primes or args.prime_powers) and not (args.primes and args.prime_powers):
        if args.primes:
            qs = primes
        if args.prime_powers:
            qs = prime_powers

    qs.sort()

    for q in qs:
        try:
            delta_t = test_generator(q, cdobble=args.cdobble)
        except KeyboardInterrupt:
            print("keyboard interrupt")
            exit(130)
    print("passed all generation tests")

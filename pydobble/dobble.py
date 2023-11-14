#!/bin/python3

import itertools
import sys

q_def = "q = pⁿ for p ∈ ℙ, n ∈ ℤ⁺"

def sieve_of_eratosthenes(limit: int):
    sieve = [i for i in range(limit+1)]
    sieve[0] = 0
    sieve[1] = 0
    primes = []
    for n in sieve:
        if n:
            m = 2*n
            while m <= limit:
                sieve[m] = 0
                m += n
            primes.append(n)
    return primes

def generate(q: int):
    if q < 2:
        raise ValueError
    sieve = sieve_of_eratosthenes(q)
    prime = 0
    n = 0
    for prime in sieve:
        if q % prime == 0:
            r = q
            while r >= prime:
                r /= prime
                n += 1
            if r == 1:
                break
            else:
                raise ValueError
    if n == 0:
        raise ValueError

    # all possible `points'
    ps = []
    for i in range(q):
        for j in range(q):
            for k in range(q):
                ps.append((i,j,k))

    # set doesn't include [0:0:0]
    ps[0] = None

    # remove all superfluous `points' (i.e. scalar multiples of another `point' modulo q)
    for i, j in itertools.combinations(range(len(ps)), 2):
        if ps[i] != None and ps[j] != None:
            if ((ps[i][1]*ps[j][2]-ps[i][2]*ps[j][1]) % prime == 0) and ((ps[i][0]*ps[j][2]-ps[i][2]*ps[j][0]) % prime == 0) and ((ps[i][0]*ps[j][1]-ps[i][1]*ps[j][0]) % prime == 0): 
                ps[j] = None
    points = []
    for p in ps:
        if p != None:
            points.append(p)

    # generate cards, associate each point with a number in [1,q]
    cards = []
    mapping = [0] * q**3
    i = 1
    for l in points: # interpret points also as lines (duality of finite projective planes)
        c = []
        for p in points:
            if (l[0]*p[0] + l[1]*p[1] + l[2]*p[2]) % q == 0:
                if mapping[p[0]*q**2+p[1]*q+p[2]] == 0:
                    mapping[p[0]*q**2+p[1]*q+p[2]] = i
                    i += 1
                c.append(mapping[p[0]*q**2+p[1]*q+p[2]])
        cards.append(c)

    return cards

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("expected one argument `q':", q_def)
        exit(1)
    else:
        try:
            q = int(sys.argv[1])
        except ValueError:
            print("invalid argument `q': should be an integer", q_def)
            exit(1)
        d = generate(q)
        print(d)

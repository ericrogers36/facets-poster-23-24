#!/bin/python3

import argparse
import copy
import math
import os
import sympy

from PIL import Image, ImageDraw, ImageFont

try:
    import cdobble as cd
    has_cdobble = True
except ImportError:
    has_cdobble = False

# paper sizes
A = lambda i: (841//math.sqrt(2**i), 1189//math.sqrt(2**i))
B = lambda i: (1000//math.sqrt(2**i), 1414//math.sqrt(2**i))
C = lambda i: (917//math.sqrt(2**i), 1297//math.sqrt(2**i))

class AlreadyExistsError(Exception):
    def __init__(self, filename, *args):
        super().__init__(args)
        self.filename = filename
    def __str__(self):
        return f"file `{self.filename}' already exists"

class LazyError(Exception):
    def __init__(self, msg, *args):
        super().__init__(args)
        self.msg = msg
    def __str__(self):
        return msg

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

class DobbleDeck:
    _default_opts = {
        "cardsize": 90,
        "dpi": 72,
        "fontcolour": "black",
        "fontfamily": "/usr/share/fonts/liberation/LiberationSerif-Regular.ttf",
        "fontsize": 48,
        "imsize": 10,
        "margin": 10,
        "papersize": A(4),
        "ppipdpi": 0.5, # (pixels per inch per dots per inch) hehe stupid variable name mwahahaha >:)
        "symbtype": "text"
    }

    def __init__(self, q, cdobble=False, mapping=None, options=None):
        if cdobble and not has_cdobble:
            raise ImportError("cdobble could not be loaded. Are you sure it has been built?")

        self._q = q
        self._cards = self._gen_deck(q, cdobble)
        self._img_cards = [None] * len(self)
        self._img_symbols = [None] * len(self)
        self._options = options or DobbleDeck._default_opts
        self._mapping = mapping or [str(i) for i in range(len(self))]
        self._requires_img_regen = True

    def __len__(self):
        return len(self._cards)

    @property
    def q(self):
        return self._q
    @property
    def cards(self):
        return copy.deepcopy(self._cards)
    @property
    def img_cards(self):
        return copy.deepcopy(self._img_cards)

    @property
    def options(self):
        return self._options
    @options.setter
    def options(self, options):
        self._options = options
        self._img_symbols = [None] * len(self)
        self._requires_img_regen = True
    def set_options(self, changes):
        for key in changes:
            self._options[key] = changes[key]
        self._img_symbols = [None] * len(self)
        self._requires_img_regen = True

    @property
    def mapping(self):
        return self._mapping
    @mapping.setter
    def mapping(self, value):
        self._mapping = value
        self._img_symbols = [None] * len(self)
        self._requires_img_regen = True

    def _gen_deck(self, q, cdobble):
        if q < 2:
            raise ValueError("`q' should be an integer at least 2")
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
                    raise ValueError("`q' is not a power of a prime")

        if cdobble:
            return cd.generate(prime, n)

        points = [0] * (q**2+q+1)
        cards = []

        if n == 1:
            field = [i for i in range(q)]

            for a in field:
                points[(q+1)*a] = (0, 1, a)
                for b in field:
                    points[(q+1)*a+b+1] = (1, a, b)
            points[q**2+q] = (0,0,1)

            # generate cards, associate each point with an integer in [0,q^2+q+1)
            mapping = [-1] * q**3
            i = 0
            for l in points: # interpret points also as lines (duality of finite projective planes)
                c = []
                for p in points:
                    if (l[0]*p[0] + l[1]*p[1] + l[2]*p[2]) % q == 0:
                        if mapping[p[0]*q**2+p[1]*q+p[2]] == -1:
                            mapping[p[0]*q**2+p[1]*q+p[2]] = i
                            i += 1
                        c.append(mapping[p[0]*q**2+p[1]*q+p[2]])
                cards.append(c)
        else: # it gets VERY slow rather quickly so just using primes is much better :)
            x = sympy.symbols("x")
            powers = [prime**i for i in range(n, -1, -1)]
            field = [sympy.poly(sum(int((i%powers[j])//powers[j+1])*x**(n-j-1) for j in range(n)), [x]) for i in range(q)]
            modulus = 0

            # slightly dubious uncited https://en.wikipedia.org/wiki/Factorization_of_polynomials_over_finite_fields#Irreducible_polynomials but it seems to work??
            for a in range(prime):
                for b in range(prime):
                    modulus = x**n + a*x + b
                    irreducible = True
                    for m in field[prime:]:
                        quot, rem = sympy.div(modulus, m)
                        if rem == 0:
                            irreducible = False
                            break
                    if irreducible:
                        break
            if not irreducible:
                raise LazyError("no irreducible polynomial over F_p of form x^n+ax+b exists :( (an irreducible polynomial does exist but I haven't got round to implementing it yet)")

            poly0 = sympy.poly(0, [x])
            poly1 = sympy.poly(1, [x])
            
            for a in range(q):
                points[(q+1)*a] = (poly0, poly1, field[a])
                for b in range(q):
                    points[(q+1)*a+b+1] = (poly1, field[a], field[b])
            points[q**2+q] = (poly0, poly0, poly1)

            mapping = [-1] * (q**6 * 2)
            i = 0
            for l in points: # interpret points also as lines (duality of finite projective planes)
                c = []
                for p in points:
                    pm = sympy.poly(sympy.rem((l[0]*p[0] + l[1]*p[1] + l[2]*p[2]).as_expr(), modulus), [x])
                    cms = sum(c % prime for c in pm.coeffs())
                    if cms == 0:
                        key = p[0].as_expr()*q**6 + p[1].eval(prime)*q**3 + p[2].eval(prime)
                        if mapping[key] == -1:
                            mapping[key] = i
                            i += 1
                        c.append(mapping[key])
                cards.append(c)

        return cards

    def _gen_card_image(self, card, ppi):
        mm_to_px = lambda m: int(m*ppi/2.54)

        img = Image.new("RGBA", (mm_to_px(self.options["cardsize"]), mm_to_px(self.options["cardsize"])), color=(255,255,255))
        draw = ImageDraw.Draw(img)
        draw.ellipse((0, 0, mm_to_px(self.options["cardsize"]), mm_to_px(self.options["cardsize"])), outline = "black", width = mm_to_px(1))

        # https://demonstrations.wolfram.com/SunflowerSeedArrangements
        pol_to_cart = lambda r, theta: (r*math.cos(theta), r*math.sin(theta))
        cart_to_scrn = lambda x, y: (x+self.options["cardsize"]/2, y+self.options["cardsize"]/2)
        theta_mult = 4*math.pi/(3+math.sqrt(5)) # 2π/φ^2
        c = 0.3*self.options["cardsize"]/math.sqrt(len(card))
        symbol_centres = [tuple(map(mm_to_px, cart_to_scrn(*pol_to_cart(c*math.sqrt(i), theta_mult*i)))) for i in range(1, len(card)+1)]

        if self.options["symbtype"] == "text":
            font = ImageFont.truetype(self.options["fontfamily"], self.options["fontsize"], encoding="unic")
            for i in range(len(card)):
                draw.text(symbol_centres[i], self.mapping[card[i]], self.options["fontcolour"], font)
        elif self.options["symbtype"] == "impath":
            if self._img_symbols[0] == None:
                for i in range(len(self)):
                    im = Image.open(self.mapping[i])
                    aspect_ratio = im.width/img.height
                    if im.width > im.height:
                        im = im.resize((self.options["imsize"]*ppi, int(self.options["imsize"]*im.height/im.width*ppi)), Image.Resampling.LANCZOS)
                    else:
                        im = im.resize((int(self.options["imsize"]*im.width/im.height*ppi), self.options["imsize"]*ppi), Image.Resampling.LANCZOS)
                    self._img_symbols[i] = im

            for i in range(len(card)):
                im = self._img_symbols[card[i]]
                img.paste(im, (int(symbol_centres[i][0]-im.width/2), int(symbol_centres[i][1]-im.height/2), int(symbol_centres[i][0]+im.width/2), int(symbol_centres[i][1]+im.height/2)))
        else:
            raise ValueError("invalid symbol type")

        return img

    def _create_imgs(self, ppi):
        if self._requires_img_regen:
            for i in range(len(self)):
                self._img_cards[i] = self._gen_card_image(self._cards[i], ppi)
            self._requires_img_regen = False

        return self

    def export_pdf(self, filename=None):
        ppi = int(self.options["ppipdpi"]*self.options["dpi"])
        mm_to_px = lambda m: int(m*ppi/2.54)

        im_size = tuple(map(mm_to_px, self.options["papersize"]))

        cards_per_line = int((self.options["papersize"][0]-2*self.options["margin"]) // self.options["cardsize"])
        card_spacing_h = 0 if cards_per_line == 1 else int(((self.options["papersize"][0]-2*self.options["margin"]) % self.options["cardsize"]) // (cards_per_line - 1))
        lines_per_page = int((self.options["papersize"][1]-2*self.options["margin"]) // self.options["cardsize"])
        card_spacing_v = 0 if lines_per_page == 1 else int(((self.options["papersize"][1]-2*self.options["margin"]) % self.options["cardsize"]) // (lines_per_page - 1))
        cards_per_page = (cards_per_line*lines_per_page)

        n_pages = int(len(self) / cards_per_page) + int((len(self) % cards_per_page) != 0)
        pages = [Image.new("RGB", im_size, (255,255,255)) for i in range(n_pages)]

        self._create_imgs(ppi)

        for p in range(n_pages):
            for l in range(lines_per_page):
                for c in range(cards_per_line):
                    card_index = p*cards_per_page+l*cards_per_line+c
                    if card_index >= len(self):
                        break;
                    pages[p].paste(self.img_cards[card_index], (mm_to_px(self.options["margin"]+c*(self.options["cardsize"]+card_spacing_h)), mm_to_px(self.options["margin"]+l*(self.options["cardsize"]+card_spacing_v))))

        pages[0].save(filename or os.getenv("PWD")+"/pydobble-"+str(self.q)+".pdf", "PDF", append_images=pages[1:], save_all=True)

        return self

    def export_imgs(self, format=None):
        if format == None:
            format = os.getenv("PWD")+"/pydobble-{q}-{i:0"+str(len(str(len(self))))+"d}.png"

        self._create_imgs(ppi=int(self.options["ppipdpi"]*self.options["dpi"]))

        for i in range(0, len(self)):
            filename = format.format(q=self.q,i=i)
            try:
                os.stat(filename)
                raise AlreadyExistsError(filename)
            except FileNotFoundError:
                pass

            self._img_cards[i].save(filename)

        return self

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("q", type=int, help="q = pⁿ for p ∈ ℙ, n ∈ ℤ⁺")
    parser.add_argument("--export-imgs", action="store_true", help="export PNG files of the individual cards instead of creating a single PDF")
    parser.add_argument("--images", nargs="+", help="q²+q+1 paths to image files to be used")
    parser.add_argument("--output-path", help="name of exported pdf file")
    parser.add_argument("--print", action="store_true", help="print list of cards to STDOUT")
    args = parser.parse_args()

    deck = DobbleDeck(args.q)

    if args.print:
        print(deck.cards)
    else:
        if args.images != None:
            if len(args.images) != args.q**2+args.q+1:
                raise ValueError(f"invalid number of images (expected {args.q**2+args.q+1}, got {len(args.images)})")
            deck.set_options({"symbtype": "impath"})
            deck.mapping = args.images

        if args.export_imgs:
            deck.export_imgs(format=args.output_path)
        else:
            deck.export_pdf(filename=args.output_path)

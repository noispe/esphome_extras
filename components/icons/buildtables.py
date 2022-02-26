#!/bin/env python3

import sys
import os
import json
from fontTools.ttLib import TTFont
import itertools

_PUA_CODEPOINTS = [
    range(0xE000, 0xF8FF + 1),
    range(0xF0000, 0xFFFFD + 1),
    range(0x100000, 0x10FFFD + 1)
]


def _is_pua(codepoint):
    return any(r for r in _PUA_CODEPOINTS if codepoint in r)


if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("input.ttf output.json")
        sys.exit(1)

    ttf_file = sys.argv[1]
    out_file = sys.argv[2]
    with TTFont(ttf_file) as ttfont:
        rev_cmap = {v: k for k, v in itertools.chain.from_iterable([t.cmap.items() for t in ttfont['cmap'].tables if t.isUnicode()]) if _is_pua(k)}
        with open(out_file, 'w') as outfile:
            json.dump(rev_cmap, outfile, indent=4, sort_keys=True)

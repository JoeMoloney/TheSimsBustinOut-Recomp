#!/usr/bin/env python3
"""Ship of Theseus progress images for the README.

Usage (from the repo root):
    python3 tools/theseus/progress.py

Writes:
    docs/progress/theseus-map.svg    one square per recompiled function, in address order ("defrag map")
    docs/progress/theseus-badge.svg  one-line counter for the top of the README

Inputs:
    work/sims-bustin-out/disc-files/SLES_520.48           the game ELF (needed for source-file labels)
    work/sims-bustin-out/SLES_520.48-recovered-map.csv     recompiler function map
    config/supplemental-map.csv                            supplemental function map
    tools/theseus/batch/ledger.tsv                         rewrite status per function (same columns
                                                           as Fable II's tools/theseus ledger)

Ledger status -> colour:
    shipped                 -> Shipped      (rewrite live in the game build)
    gated, reviewed         -> Diff-tested  (rewrite passed the equivalence gate)
    anything else listed    -> In progress  (e.g. candidate, in-progress, gate-fail)
    rejected / not listed   -> Original     (still the machine-generated code)
The function address is taken from the hex suffix of the ledger `name`
(e.g. `FUN_002c85e0` or `bstring_compare_002c85e0`).

Source-file labels (no symbols or RTTI in this game): strings naming a source file
(assert messages such as `bstring.cpp`) are found in the ELF, then `lui`+`addiu/ori`
pairs that build their address mark the function using them. The linker keeps one
file's functions together, so the first such function is where the label points.

Standard library only. Each SVG carries its own light and dark colours.
"""
import csv
import datetime
import math
import os
import re
from html import escape

import elfscan

REPO = elfscan.REPO
LEDGER = os.path.join(REPO, "tools/theseus/batch/ledger.tsv")
OUT_DIR = os.path.join(REPO, "docs/progress")

# Known Sony SDK entry points, labelled on the map (addresses from config/native-bindings.csv).
SDK_LABELS = (("Sony libmpeg (HLE)", 0x3776B8), ("Sony libkernel", 0x396B50))
SIZE_CAP = 65536  # the last map entry runs into data; cap any single function's bytes

ORIGINAL, PROGRESS, VERIFIED, SHIPPED = range(4)
STAGE_NAMES = ["Original", "In progress", "Diff-tested", "Shipped"]
STATUS_STAGE = {"shipped": SHIPPED, "gated": VERIFIED, "reviewed": VERIFIED, "rejected": ORIGINAL}
FONT = 'system-ui,-apple-system,"Segoe UI",sans-serif'

# Red -> amber -> green. Checked with the dataviz palette validator (light, all pairs): every pair
# stays apart under simulated red-green colour blindness (worst Delta E 8.5, target >= 8) and for
# normal vision (worst 16.9, floor 15). Salmon and dark green also differ strongly in lightness.
STAGES_LIGHT = ["#e9897b", "#f0c64a", "#3b8f42", "#1a5a26"]
STAGES_DARK = ["#8a3a32", "#c29a2a", "#3b8f42", "#6aa86a"]
INK_LIGHT = {"bg": "#fcfcfb", "t1": "#0b0b0b", "t2": "#52514e", "t3": "#898781"}
INK_DARK = {"bg": "#1a1a19", "t1": "#f0efec", "t2": "#c3c2b7", "t3": "#898781"}


# ---------------------------------------------------------------- data


def source_file_labels(funcs):
    """Return {source .cpp file: index of the first function that references it}."""
    first = {}
    for k, names in elfscan.source_refs(funcs, r"([A-Za-z0-9_]+\.cpp)\b").items():
        for name in names:
            first[name] = min(first.get(name, k), k)
    return first


def load_states(funcs):
    index = {a: k for k, (a, _) in enumerate(funcs)}
    states = [ORIGINAL] * len(funcs)
    unknown = []
    if not os.path.exists(LEDGER):
        return states, unknown
    with open(LEDGER) as f:
        for row in csv.DictReader(f, delimiter="\t"):
            m = re.search(r"_(?:0x)?([0-9a-fA-F]{6,8})$", row["name"])
            addr = int(m.group(1), 16) if m else None
            if addr not in index:
                unknown.append(row["name"])
                continue
            states[index[addr]] = STATUS_STAGE.get(row["status"].strip(), PROGRESS)
    return states, unknown


# ---------------------------------------------------------------- drawing


def style():
    def block(ink, stages):
        rules = [f".bg{{fill:{ink['bg']}}}", f".t1{{fill:{ink['t1']}}}", f".t2{{fill:{ink['t2']}}}",
                 f".t3{{fill:{ink['t3']}}}", f".ln{{stroke:{ink['t3']}}}", f".mk{{stroke:{ink['t1']}}}"]
        return "".join(rules + [f".s{i}{{fill:{c}}}" for i, c in enumerate(stages)])

    return (f"<style>text{{font-family:{FONT}}}{block(INK_LIGHT, STAGES_LIGHT)}"
            f"@media (prefers-color-scheme:dark){{{block(INK_DARK, STAGES_DARK)}}}</style>")


def text(x, y, s, cls="t1", size=13, weight=400, anchor="start"):
    return (f'<text x="{x}" y="{y}" class="{cls}" font-size="{size}" font-weight="{weight}" '
            f'text-anchor="{anchor}">{escape(s)}</text>')


def fmt_bytes(n):
    return f"{n / 1048576:.1f} MB" if n >= 1048576 else f"{n / 1024:.1f} KB"


def defrag_map(funcs, states, labels):
    n = len(funcs)
    cols, pitch, cell = 128, 8, 7
    rows = math.ceil(n / cols)
    gx, gy = 78, 92
    gw, gh = cols * pitch, rows * pitch
    W, H = gx + gw + 210, gy + gh + 56

    count = [0] * 4
    size = [0] * 4
    paths = [[] for _ in range(4)]
    for i, s in enumerate(states):
        count[s] += 1
        size[s] += min(funcs[i][1], SIZE_CAP)
        r, c = divmod(i, cols)
        paths[s].append(f"M{gx + c * pitch} {gy + r * pitch}h{cell}v{cell}h-{cell}z")

    o = [f'<rect class="bg" width="{W}" height="{H}" rx="8"/>',
         text(28, 38, "Ship of Theseus · SLES_520.48 code, in address order", "t1", 20, 600),
         text(28, 60, f"One square per recompiled function ({n:,}). Red is machine-generated code; "
                      f"green has been rewritten by hand and proven equivalent.", "t3", 12)]
    x = 28
    for s in (SHIPPED, VERIFIED, PROGRESS, ORIGINAL):  # legend with counts
        label = f"{STAGE_NAMES[s]}  {count[s]:,}"
        o.append(f'<rect x="{x}" y="70" width="12" height="12" rx="2" class="s{s}"/>')
        o.append(text(x + 18, 80, label, "t2", 12))
        x += 30 + len(label) * 7
    o += [f'<path class="s{s}" d="{"".join(p)}"/>' for s, p in enumerate(paths) if p]
    for r in range(0, rows, 8):  # address ticks
        o.append(text(gx - 8, gy + r * pitch + 7, f"0x{funcs[r * cols][0]:06x}", "t3", 10, anchor="end"))

    marks = dict(labels)
    for name, addr in SDK_LABELS:
        marks[name] = min(range(n), key=lambda i: abs(funcs[i][0] - addr))
    last = -99
    for name, i in sorted(marks.items(), key=lambda kv: kv[1]):
        r, col = divmod(i, cols)
        cx, cy = gx + col * pitch, gy + r * pitch
        ty = max(cy + 4, last + 13)  # push labels down so they never overlap
        last = ty
        o.append(f'<rect x="{cx - 1.5}" y="{cy - 1.5}" width="{cell + 3}" height="{cell + 3}" rx="2" '
                 f'fill="none" class="mk" stroke-width="1.5"/>')
        o.append(f'<path class="ln" fill="none" stroke-width="1" d="M{gx + gw + 2} {cy + 4}'
                 f'L{gx + gw + 18} {ty}H{gx + gw + 22}"/>')
        o.append(text(gx + gw + 26, ty + 4, name, "t2", 11))

    today = datetime.date.today().isoformat()
    o.append(text(28, H - 22, f"{count[SHIPPED]:,} / {n:,} functions shipped · {fmt_bytes(size[SHIPPED])} / "
                              f"{fmt_bytes(sum(size))} of code · labels from assert strings · updated {today}",
                  "t2", 12))
    title = f"Ship of Theseus progress: {count[SHIPPED]:,} of {n:,} functions shipped"
    return (f'<svg xmlns="http://www.w3.org/2000/svg" width="{W}" height="{H}" viewBox="0 0 {W} {H}" '
            f'role="img" aria-label="{escape(title)}"><title>{escape(title)}</title>{style()}'
            f'{"".join(o)}</svg>\n')


def badge(shipped, total):
    left, right = "theseus", f"{shipped:,} / {total:,} shipped"
    lw, rw = 14 + len(left) * 7, 14 + len(right) * 7
    w = lw + rw
    return (f'<svg xmlns="http://www.w3.org/2000/svg" width="{w}" height="20" role="img" '
            f'aria-label="theseus: {right}"><title>theseus: {right}</title>'
            f'<clipPath id="r"><rect width="{w}" height="20" rx="3"/></clipPath><g clip-path="url(#r)">'
            f'<rect width="{lw}" height="20" fill="#52514e"/>'
            f'<rect x="{lw}" width="{rw}" height="20" fill="#1a5a26"/></g>'
            f'<g fill="#fff" font-family=\'{FONT}\' font-size="11" text-anchor="middle">'
            f'<text x="{lw / 2}" y="14">{left}</text><text x="{lw + rw / 2}" y="14">{right}</text></g></svg>\n')


def main():
    funcs = elfscan.load_functions()
    labels = source_file_labels(funcs)
    states, unknown = load_states(funcs)
    os.makedirs(OUT_DIR, exist_ok=True)
    shipped = states.count(SHIPPED)
    for name, content in (("theseus-map.svg", defrag_map(funcs, states, labels)),
                          ("theseus-badge.svg", badge(shipped, len(funcs)))):
        with open(os.path.join(OUT_DIR, name), "w") as f:
            f.write(content)
    print(f"{len(funcs):,} functions, {shipped} shipped, {len(labels)} source files labelled "
          f"-> {os.path.relpath(OUT_DIR, REPO)}/theseus-map.svg, theseus-badge.svg")
    for name in unknown:
        print(f"warning: ledger row {name!r} does not match a function start; not drawn")


if __name__ == "__main__":
    main()

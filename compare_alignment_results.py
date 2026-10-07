#!/usr/bin/env python3
"""Compare bench_alignment_vectorization results (Google Benchmark JSON, run with --benchmark_repetitions and
--benchmark_report_aggregates_only=true).

Usage:
    compare_alignment_results.py results/alignment_default.json [results/alignment_native.json]

For each kernel and size, prints the median time of the reference layout (Aligned64, ie OBJEXXFCL_ALIGN=64) and the
other layouts as a % difference to it. With two files (default build, then native build), also prints the native speedup
on the reference layout. Layouts and sizes are discovered from the files, so new ones are picked up automatically.
"""

import argparse
import json
import re
from collections import defaultdict
from pathlib import Path

REFERENCE_LAYOUT = "Aligned64"
RUN_NAME_RE = re.compile(r"BM_(\w+)<Layout::(\w+)>/(\d+)")


def load(path: Path):
    """Return ({(kernel, size): {layout: median_ns}}, {(kernel, size, layout): {counter: value}}, max cv, context)."""
    data = json.loads(path.read_text())
    medians = defaultdict(dict)
    counters = {}
    max_cv = 0.0
    for b in data["benchmarks"]:
        aggregate = b.get("aggregate_name")
        m = RUN_NAME_RE.match(b["run_name"])
        if not m or aggregate not in ("median", "cv"):
            continue
        kernel, layout, size = m.group(1), m.group(2), int(m.group(3))
        if aggregate == "median":
            medians[(kernel, size)][layout] = b["real_time"]
            counters[(kernel, size, layout)] = {k: v for k, v in b.items() if k.endswith("_mod_128")}
        else:
            max_cv = max(max_cv, b["real_time"])
    return medians, counters, max_cv, data.get("context", {})


def layout_order(all_layouts):
    preferred = ["Default", "Aligned64", "Aligned128", "Offset8", "Misaligned8", "Offset16", "Offset32"]
    return [l for l in preferred if l in all_layouts] + sorted(set(all_layouts) - set(preferred))


def print_table(title, medians, other=None):
    layouts = layout_order({l for v in medians.values() for l in v})
    print(f"\n## {title}\n")
    header = f"| kernel | n | {REFERENCE_LAYOUT} (ns) |"
    sep = "|---|---|---|"
    if other is not None:
        header += " vs default build |"
        sep += "---|"
    for l in layouts:
        if l != REFERENCE_LAYOUT:
            header += f" {l} |"
            sep += "---|"
    print(header)
    print(sep)
    for kernel, size in sorted(medians):
        row = medians[(kernel, size)]
        ref = row.get(REFERENCE_LAYOUT)
        line = f"| {kernel} | {size} | {ref:,.1f} |" if ref else f"| {kernel} | {size} | - |"
        if other is not None:
            other_ref = other.get((kernel, size), {}).get(REFERENCE_LAYOUT)
            line += f" {other_ref / ref:.2f}x faster |" if ref and other_ref else " - |"
        for l in layouts:
            if l == REFERENCE_LAYOUT:
                continue
            line += f" {(row[l] / ref - 1) * 100:+.0f}% |" if ref and l in row else " - |"
        print(line)


def print_alignments(counters):
    """Actual first-element offset (mod 128 bytes) per layout and buffer, to catch 'lucky' allocations."""
    seen = defaultdict(set)
    for (kernel, size, layout), c in counters.items():
        for name, value in c.items():
            seen[(layout, size, name)].add(int(value))
    order = {l: i for i, l in enumerate(layout_order({k[0] for k in seen}))}
    print("\n## Observed offsets mod 128 bytes (per layout, size, buffer)\n")
    for layout, size, name in sorted(seen, key=lambda k: (order[k[0]], k[1], k[2])):
        print(f"- {layout} n={size} {name}: {sorted(seen[(layout, size, name)])}")


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("default_json", type=Path, help="Results of bench_alignment_vectorization")
    parser.add_argument("native_json", type=Path, nargs="?", help="Results of bench_alignment_vectorization_native")
    parser.add_argument("--offsets", action="store_true", help="Also list the observed buffer offsets")
    args = parser.parse_args()

    default, default_counters, default_cv, ctx = load(args.default_json)
    print(f"Host: {ctx.get('host_name')}, {ctx.get('num_cpus')} CPUs. Median times; % is relative to {REFERENCE_LAYOUT}.")
    print(f"Max coefficient of variation: default {default_cv:.1%}", end="")
    native = None
    if args.native_json:
        native, native_counters, native_cv, _ = load(args.native_json)
        print(f", native {native_cv:.1%}")
    else:
        print()

    print_table(f"Default build ({args.default_json.name})", default)
    if native is not None:
        print_table(f"Native build ({args.native_json.name})", native, other=default)
    if args.offsets:
        print_alignments(default_counters)


if __name__ == "__main__":
    main()

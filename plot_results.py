"""Run and plot results for a benchmark.

Usage:
  plot_results.py BENCH_NAME
  plot_results.py (-l | --list)
  plot_results.py (-h | --help)

Options:
  -h --help     Show this screen.
  -l --list     List available benchmark executables

Example:
  plot_results.py bench_basics
"""
from docopt import docopt
import subprocess
from pathlib import Path
import shlex
import glob as gb
import pandas as pd
import matplotlib.pyplot as plt

ROOT_DIR = Path(__file__).parent.absolute()


def infer_products_dir() -> Path:
    """
    Locate the build/Products/ directory under the current root

    Returns:
    --------
    * products_dir (pathlib.Path): the path to the Products directory
    """
    matches = gb.glob(str(ROOT_DIR / '**/Products'), recursive=True)
    matches = [x for x in matches if Path(x).is_dir()]
    if len(matches) == 0:
        raise IOError(
            "Couldn't locate build dir, looked everywhere for a 'Products'"
            "directory but came up empty. Have you built the project?")
    if len(matches) > 1:
        print("Found multiple 'Products' directories, returning the first ="
              f"'{matches[0]}'"
              f"Other potential matches: {matches[1:]}")
    return Path(matches[0])


def list_benchmarks(products_dir: Path) -> list[str]:
    """
    Lists the benchmarks inside a Products directory by looking at any file
    starting with bench_*

    Returns:
    --------

    bench_names: the names of the executables
    """

    matches = gb.glob(str(products_dir / 'bench_*'), recursive=False)
    matches = [Path(p).relative_to(products_dir) for p in matches]
    return matches


if __name__ == '__main__':
    arguments = docopt(__doc__)
    print(arguments)

    root_dir = Path(__file__).parent.absolute()
    products_dir = infer_products_dir()

    if arguments['--list']:
        print("The following benchmark executables are available:")
        for b in list_benchmarks(products_dir):
            print(f"* {b}")
        exit(0)

    bench_name = arguments['BENCH_NAME']

    bench_exe = products_dir / bench_name
    if not bench_exe.exists():
        raise ValueError(
            f"Couldn't find a benchmark executable named {bench_exe} "
            f"in {products_dir}. Available benchmarks: {list_benchmarks()}"
        )

    results_dir = root_dir / "results"
    results_dir.mkdir(exist_ok=True)

    results_file = results_dir / f"{bench_name}.csv"

    cmd = shlex.split(
        f'{bench_exe} --benchmark_out_format=csv'
        f' --benchmark_out="{results_file}"')
    subprocess.check_call(cmd)
    df = pd.read_csv(results_file, skiprows=10, index_col=0)
    fig, ax = plt.subplots(figsize=(16, 9))
    df[['real_time', 'cpu_time']].plot(kind='bar', ax=ax)
    time_units = df['time_unit'].unique()
    ax.set_title(bench_name)
    if len(time_units) == 1:
        ax.set_ylabel(time_units[0])
    else:
        raise NotImplementedError(
            "There seems to be multiple time units, which I haven't "
            f"encountered before and therefore didn't treat: {time_units}"
        )
    fig.tight_layout()
    plt.show()

#!/usr/bin/env python3
"""Run a seeded, strictly validated comparison and record local provenance."""
import argparse, csv, hashlib, json, os, pathlib, platform, subprocess

root = pathlib.Path(__file__).resolve().parents[1]
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--output', type=pathlib.Path, default=root/'build/benchmarks')
p.add_argument('--seed', type=int, default=123)
p.add_argument('--trials', type=int, default=20)
a = p.parse_args()
if a.trials < 1: p.error('--trials must be positive')
out = a.output.resolve(); out.mkdir(parents=True, exist_ok=True)
subprocess.run(['make','build/bf_suite'],cwd=root,check=True)
env = dict(os.environ, SEED=str(a.seed), BF_TRIALS=str(a.trials), BF_OUTPUT=str(out/'summary.csv'), BF_RAW_OUTPUT=str(out/'raw.csv'), BF_CORPUS=str(out/'corpus.csv'))
subprocess.run([str(root/'build/bf_suite')],cwd=root,env=env,check=True)
cc=os.environ.get('CC','cc')
flags=os.environ.get('CFLAGS','-O2 -Wall -Wextra -Wshadow -pedantic -std=c11 -g')
cpu=platform.processor()
if pathlib.Path('/proc/cpuinfo').exists():
    cpu=next((l.split(':',1)[1].strip() for l in pathlib.Path('/proc/cpuinfo').read_text().splitlines() if l.startswith('model name')),cpu)
metadata={'seed':a.seed,'trials_per_hole_count':a.trials,'hole_counts':[30,40,50,60], 'compiler':subprocess.check_output([cc,'--version'],text=True).splitlines()[0], 'cflags':flags,'platform':platform.platform(),'libc':platform.libc_ver(),'cpu':cpu,'timer':'clock_gettime(CLOCK_MONOTONIC)','unit':'milliseconds','protocol':'One solve per solver and puzzle; rotating execution order; generation and validation excluded; all outputs complete, clue-preserving; each puzzle unique by capped DLX count.','rng':'C rand(); seed is repeatable within a libc implementation, not a cross-platform corpus identity.','corpus_sha256':hashlib.sha256((out/'corpus.csv').read_bytes()).hexdigest(),'revision_base':subprocess.check_output(['git','rev-parse','HEAD'],cwd=root,text=True).strip()}
(out/'environment.json').write_text(json.dumps(metadata,indent=2)+'\n')
print(out)

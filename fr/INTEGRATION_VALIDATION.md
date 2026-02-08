# Plan d’Intégration & Validation (FR)
**Binôme :** ODE_BRINO_Rafael_RIZO_Maria_Eduarda — **Date :** 2025-10-17

## 0) Pré-requis
- Linux, `make`, `cc` (C11), `SDL2` (+ `SDL2_ttf` si utilisé).

## 1) Cibles de build
```bash
make clean && make cli
make gui
make test
```
**Attendu :** `build/sudoku_cli`, `build/sudoku_gui` présents ; pas d’erreur ; avertissements acceptables (aucun bloquant).  
`make test` lance `test_basic` et `bf_suite` (mettre `BF_TRIALS=200` pour les runs lourds).

## 2) Tests CLI
```bash
./build/sudoku_cli --generate 40 > k40.txt
./build/sudoku_cli --solve k40.txt --solver dlx > solved.txt
./build/sudoku_cli --solve k40.txt --solver mrv > solved_mrv.txt
```
**Note :** le générateur vise `k` dans `0..60` pour garantir l’unicité.
**Vérifs :** grille valide (lignes/colonnes/boîtes), solution stable d’un run à l’autre, temps ≤ 1s pour k≤50 (indicatif).  
Sélecteur de solveur : `--solver auto|dlx|bt|mrv|cp`.

## 3) Unicité
Générer 10 grilles par k∈{30,40,50} ; DLX doit trouver exactement une solution.

```bash
for k in 30 40 50; do
  for i in $(seq 1 10); do ./build/sudoku_cli --generate $k > tmp.txt && ./build/sudoku_cli --solve tmp.txt >/dev/null; done
done
```

## 4) Sauvegarde/chargement
- GUI : saisir quelques chiffres → **K** (sauver) → relancer → **L** (charger).

- **Attendu :** état chargé identique à l’état sauvegardé.

## 5) Indice
- GUI : **H** révèle une case légale ; puzzle reste solvable.

## 6) Trace / pas-à-pas
- GUI : **B** pour suivre la trace du backtracking ; événements TRY/ASSIGN/BACKTRACK visibles ; solution finale cohérente.

## 7) Tests négatifs
- Fichier invalide → message clair + code retour ≠ 0.

- Grille non unique → détectée (via mode comptage) ou empêchée par le générateur.

## 8) Livrables
- Rapport (*rapport_en.md* / *rapport_fr.md*).

- Ce plan (EN/FR).

- Justification algorithmique (EN/FR).

- Architecture (EN/FR).

- Makefile avec `cli`, `gui`, `bench` (optionnel).

## 9) Reproductibilité
- Option : graine RNG fixe via env ou CLI.
  - `SEED=123 ./build/sudoku_cli --generate 40 > k40.txt`
  - `./build/sudoku_cli --generate 40 --seed 123 > k40.txt`

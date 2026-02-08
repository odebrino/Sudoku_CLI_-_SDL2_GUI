#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="${1:-$ROOT/assets/screenshots/gui.png}"
TMP="$(mktemp /tmp/sudoku_gui_XXXX.bmp)"

mkdir -p "$(dirname "$OUT")"

echo "== Build GUI =="
make -C "$ROOT" gui

echo "== Capture GUI screenshot =="
GUI_SCREENSHOT="$TMP" xvfb-run -a "$ROOT/build/sudoku_gui" || true

python3 - <<PY
from PIL import Image
img = Image.open("$TMP")
img.save("$OUT")
PY

rm -f "$TMP"
echo "Saved $OUT"

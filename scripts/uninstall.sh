#!/bin/sh
set -eu
rm -f "$HOME/.local/bin/se"
rm -rf "$HOME/.local/share/se"
for rc in "$HOME/.profile" "$HOME/.zshrc"; do
  [ -f "$rc" ] || continue
  tmp="$rc.se-uninstall.$$"
  awk 'BEGIN{skip=0} $0=="# SE language PATH"{skip=1;next} skip==1 && $0=="export PATH=\"$HOME/.local/bin:$PATH\""{skip=0;next} {print}' "$rc" > "$tmp"
  mv "$tmp" "$rc"
done
printf 'SE has been uninstalled.\n'

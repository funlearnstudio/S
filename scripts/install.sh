#!/bin/sh
set -eu
REPO="funlearnstudio/S"
BASE="$HOME/.local/share/se"
BIN="$HOME/.local/bin"
VERSION="${1:-latest}"
os=$(uname -s)
arch=$(uname -m)
case "$os:$arch" in
  Linux:x86_64|Linux:amd64) asset="se-linux-x86_64.tar.gz" ;;
  Darwin:x86_64) asset="se-macos-x86_64.tar.gz" ;;
  Darwin:arm64|Darwin:aarch64) asset="se-macos-arm64.tar.gz" ;;
  *) echo "SE does not publish a binary for $os/$arch yet." >&2; exit 1 ;;
esac
if [ "$VERSION" = latest ]; then
  url="https://github.com/$REPO/releases/latest/download/$asset"
else
  case "$VERSION" in v*) tag="$VERSION" ;; *) tag="v$VERSION" ;; esac
  url="https://github.com/$REPO/releases/download/$tag/$asset"
fi
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
curl -fL "$url" -o "$tmp/se.tar.gz"
rm -rf "$BASE"
mkdir -p "$BASE" "$BIN"
tar -xzf "$tmp/se.tar.gz" -C "$BASE"
ln -sf "$BASE/bin/se" "$BIN/se"
marker='# SE language PATH'
line='export PATH="$HOME/.local/bin:$PATH"'
for rc in "$HOME/.profile" "$HOME/.zshrc"; do
  [ -e "$rc" ] || : > "$rc"
  if ! grep -F "$marker" "$rc" >/dev/null 2>&1; then
    printf '\n%s\n%s\n' "$marker" "$line" >> "$rc"
  fi
done
printf 'Installed SE to %s\n' "$BASE"
printf 'Open a new terminal, then run: se --version\n'

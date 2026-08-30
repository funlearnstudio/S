#!/usr/bin/env sh
set -eu

VERSION="${SE_VERSION:-0.6.0}"
REPO="funlearnstudio/SE"
TAG="v$VERSION"
BASE_URL="https://github.com/$REPO/releases/download/$TAG"
INSTALL_ROOT="${SE_INSTALL_ROOT:-$HOME/.local/share/se}"
VERSION_DIR="$INSTALL_ROOT/$VERSION"
BIN_DIR="${SE_BIN_DIR:-$HOME/.local/bin}"

os="$(uname -s)"
arch="$(uname -m)"

case "$os" in
  Darwin) platform="macos" ;;
  Linux) platform="linux" ;;
  *) echo "SE installer: unsupported operating system: $os" >&2; exit 1 ;;
esac

case "$arch" in
  x86_64|amd64) machine="x64" ;;
  arm64|aarch64) machine="arm64" ;;
  *) echo "SE installer: unsupported architecture: $arch" >&2; exit 1 ;;
esac

if [ "$platform" = "linux" ] && [ "$machine" = "arm64" ]; then
  echo "SE $VERSION currently has no prebuilt Linux arm64 package." >&2
  exit 1
fi

asset="se-$VERSION-$platform-$machine"
archive="$asset.tar.gz"
url="$BASE_URL/$archive"

tmp="$(mktemp -d 2>/dev/null || mktemp -d -t se-install)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM

fetch() {
  if command -v curl >/dev/null 2>&1; then
    curl -fL --retry 3 --connect-timeout 15 "$1" -o "$2"
  elif command -v wget >/dev/null 2>&1; then
    wget -O "$2" "$1"
  else
    echo "SE installer needs curl or wget only to download the prebuilt package." >&2
    exit 1
  fi
}

echo "Installing SE $VERSION for $platform-$machine..."
fetch "$url" "$tmp/$archive"
tar -xzf "$tmp/$archive" -C "$tmp"

mkdir -p "$INSTALL_ROOT" "$BIN_DIR"
rm -rf "$VERSION_DIR"
mv "$tmp/$asset" "$VERSION_DIR"
ln -sfn "$VERSION_DIR/bin/se" "$BIN_DIR/se"

case ":$PATH:" in
  *":$BIN_DIR:"*) ;;
  *)
    echo
    echo "SE was installed, but $BIN_DIR is not currently in PATH."
    echo "Add this line to ~/.zshrc or ~/.bashrc:"
    echo "  export PATH=\"$BIN_DIR:\$PATH\""
    ;;
esac

echo
echo "Installed: $BIN_DIR/se"
"$BIN_DIR/se" --version
printf '%s\n' 'Try: se run hello.se'
printf '%s\n' 'No CMake, Git, or C++ compiler is required for se run/check/test.'
printf '%s\n' 'Native `se build` still requires a system C++20 compiler.'

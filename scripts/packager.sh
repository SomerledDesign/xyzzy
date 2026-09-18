#!/bin/sh
set -eu

script_dir=$(CDPATH= cd -- "$(dirname "$0")" && pwd)
root_dir=$(CDPATH= cd -- "$script_dir/.." && pwd)
cd "$root_dir"

version=$(sed -n 's/^AC_INIT(\[xyzzy\], \[\([^]]*\)\],.*/\1/p' configure.ac)

if [ -z "$version" ]; then
    echo "packager.sh: unable to determine package version from configure.ac" >&2
    exit 1
fi

mkdir -p dist

# Checked-in Autotools outputs must stay newer than their sources so packaging
# does not try to regenerate them without a local autotools toolchain.
touch aclocal.m4
touch configure
touch Makefile.in
touch Makefile

echo "Building xyzzy-$version.tar.gz"
make dist
mv -f "xyzzy-$version.tar.gz" "dist/xyzzy-$version.tar.gz"
echo "Created $root_dir/dist/xyzzy-$version.tar.gz"

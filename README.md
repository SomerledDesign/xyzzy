# xyzzy

`xyzzy` is a small command-line utility in the spirit of classic text adventures.
It exists to give a developer a brief smile when they type `xyzzy` at a shell prompt,
with optional themes, seeds, and an "epic" mode for a more theatrical readout.

Version `0.7.1`

Copyright (c) 1998 Somerled Design.
Licensed under the GNU General Public License, version 2.

## Build

```sh
make
```

## Install

The default installation prefix is `/usr/local`. Installing there generally
requires administrator privileges for the documentation and manual page, even
if the executable directory is writable:

```sh
./configure
make
sudo make install
```

For an installation in your home directory without administrator privileges:

```sh
./configure --prefix="$HOME/.local"
make
make install
```

Add the executable directory to your shell's `PATH` if needed:

```sh
export PATH="$HOME/.local/bin:$PATH"
```

Use `DESTDIR` to stage a package payload:

```sh
make install DESTDIR=/tmp/xyzzy-stage
```

## Use

```sh
xyzzy
xyzzy --epic
xyzzy --theme grue --times 2
xyzzy --list
```

## Layout

- `xyzzy.c` — program source
- `man/xyzzy.1` — section 1 manual page
- `spec/prd.md` — product notes
- `homebrew/xyzzy.rb` — Homebrew formula draft (copy into the tap when releasing)
- `dist/` — output of `scripts/packager.sh`
- `scripts/packager.sh` — builds `dist/xyzzy-VERSION.tar.gz`

## Package Contents

- `bin/xyzzy`: the command
- `share/doc/xyzzy/README.md`: quick-start documentation
- `share/doc/xyzzy/LICENSE`: GNU General Public License version 2
- `share/man/man1/xyzzy.1`: manual page in the user commands section

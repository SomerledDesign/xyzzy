# xyzzy — product notes

Version: 0.7.1

## Purpose

A tiny command-line utility that prints short adventure-style lines when
invoked at a shell prompt. Optional flags select theme, repeat count, seed,
list mode, and an “epic” presentation.

## Non-goals

- Not a full text adventure or parser
- Not networked; no external assets at runtime

## CLI

| Flag | Behavior |
|------|----------|
| (none) | One omen from theme `all` |
| `--theme THEME` | `all`, `classic`, `maze`, `oracle`, `treasure`, `glitch`, `grue`, `epic` |
| `--times N` | Print N omens (N ≥ 1) |
| `--seed VALUE` | Deterministic PRNG seed (uint32) |
| `--epic` | Header/footer; default times = 3 if `--times` omitted |
| `--list` | Print matching omens (no shuffle required for listing) |
| `--help` / `-h` | Usage |
| `--version` | Version + copyright + GPLv2 notice |

## Implementation notes

- Language: C (C11 flags in the Autotools build)
- PRNG: xorshift32 (same constants as historical builds)
- License: GPLv2
- Man page: `man/xyzzy.1`
- Homebrew formula draft: `homebrew/xyzzy.rb`
- Dist tarball: `scripts/packager.sh` → `dist/xyzzy-VERSION.tar.gz`

# alcy programming language

wip.

## Build instruction

### Using Nix (Linux / Mac)

```bash
nix develop
uv run ./build/scripts/build.py

# To run tests:
# uv run ./build/scripts/run.py --target=tests
```

### Not using Nix

Install GN, ninja, clang, lld, libc++ and then run:

```bash
uv run ./build/scripts/build.py

# Or you can build without uv / python:
# ./build/scripts/build.sh

# To run tests:
# uv run ./build/scripts/run.py --target=tests
```

### Build script arguments

```bash
# Build default target with release profile in `out/build_release`:
uv run ./build/scripts/build.py --target=default --mode=release --build-subdir=build_release

# Build all targets with debug profile:
uv run ./build/scripts/build.py --target=all --mode=debug

# Build and run tests with debug profile:
uv run ./build/scripts/run.sh --target=tests --mode=debug
```

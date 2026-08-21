# alcy programming language

wip.

## Build instruction

### Using Nix (Linux / Mac)

```bash
nix develop
./build/scripts/build.sh default debug build

# To run tests:
# ./build/scripts/run.sh tests debug build
```

### Not using Nix

Install GN, ninja, clang, lld, libc++ and then run:

```bash
./build/scripts/build.sh default debug build

# To run tests:
# ./build/scripts/run.sh tests debug build
```

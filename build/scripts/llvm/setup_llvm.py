#!/usr/bin/env python3

# Copyright 2026 pugur
# This source code is licensed under the
# which can be found in the LICENSE file.

import argparse
import sys
import os
import platform

import download_llvm
import build_llvm

script_dir = os.path.dirname(__file__)
root_dir = os.path.dirname(os.path.dirname(os.path.dirname(script_dir)))

default_llvm_root_dir = os.path.join(root_dir, "third_party", "llvm", "src")
default_llvm_src_dir = os.path.join(default_llvm_root_dir, "llvm")

default_llvm_alcy_dir = os.path.join(default_llvm_root_dir, ".alcy")
default_llvm_configure_script = os.path.join(default_llvm_alcy_dir, "configure.sh")
default_llvm_build_dir = os.path.join(default_llvm_alcy_dir, "build")
default_llvm_install_dir = os.path.join(default_llvm_alcy_dir, "install")
default_llvm_download_dir = os.path.join(default_llvm_build_dir, "download")

default_tag_cache_file = os.path.join(script_dir, ".llvm_tag_cache")

default_compiler_launcher = "ccache"
default_cc = "clang"
default_cxx = "clang++"
default_generator = "Ninja"


def host_triple():
    # Detect Architecture
    arch = platform.machine().lower()
    # Normalize arch names to match LLVM conventions
    arch_map = {
        "amd64": "x86_64",
        "x86_64": "x86_64",
        "arm64": "aarch64",
        "aarch64": "aarch64",
    }
    arch = arch_map.get(arch, arch)

    # Detect OS and Vendor/ABI
    system = platform.system().lower()

    if system == "darwin":
        return f"{arch}-apple-darwin"

    elif system == "windows":
        # We assume MSVC for Python-based builds on Windows
        return f"{arch}-pc-windows-msvc"

    elif system == "linux":
        # Check for GNU vs MUSL
        # Using 'ldd --version' to check the C library
        try:
            import subprocess

            ldd_out = subprocess.check_output(
                ["ldd", "--version"], stderr=subprocess.STDOUT
            ).decode()
            if "musl" in ldd_out.lower():
                return f"{arch}-unknown-linux-musl"
        except Exception:
            pass  # Fallback to gnu
        return f"{arch}-unknown-linux-gnu"

    return f"{arch}-unknown-unknown"


def current_tag(src_dir):
    if not os.path.isdir(src_dir):
        return ""
    import subprocess

    result = subprocess.run(
        ["git", "describe", "--tags"], cwd=src_dir, capture_output=True
    )
    return result.stdout.decode().strip()


def write_tag_cache(tag, cache_file):
    try:
        with open(cache_file, "w") as f:
            f.write(tag.strip())
    except Exception as e:
        print(f"Warning: Failed to update tag cache file: {e}")


def main():
    parser = argparse.ArgumentParser(description="Setups LLVM.")

    parser.add_argument(
        "--src-dir", default=default_llvm_src_dir, help="LLVM source directory"
    )
    parser.add_argument(
        "--build-dir", default=default_llvm_build_dir, help="LLVM build directory"
    )
    parser.add_argument(
        "--install-dir",
        default=default_llvm_install_dir,
        help="LLVM installation directory",
    )
    parser.add_argument(
        "--configure-script",
        default=default_llvm_configure_script,
        help="Path to the configure.sh script",
    )
    parser.add_argument(
        "--tag-cache-file",
        default=default_tag_cache_file,
        help="Path to the tag cache file",
    )

    # Build Options
    parser.add_argument(
        "--type",
        default="Release",
        choices=["Debug", "Release", "RelWithDebInfo", "MinSizeRel"],
        help="Build type",
    )
    parser.add_argument(
        "--targets",
        default="all",
        help="LLVM targets to build (semicolon-separated, case sensitive)",
    )
    parser.add_argument(
        "--triple",
        default=host_triple(),
        help="Target triple (e.g., x86_64-unknown-linux-gnu)",
    )
    parser.add_argument(
        "--compiler-launcher",
        default=default_compiler_launcher,
        help="C/C++ compiler launcher (e.g., ccache)",
    )
    parser.add_argument(
        "--cc",
        default=default_cc,
        help="C compiler (e.g., clang, gcc)",
    )
    parser.add_argument(
        "--cxx",
        default=default_cxx,
        help="C++ compiler (e.g., clang++, g++)",
    )
    parser.add_argument(
        "--generator",
        default=default_generator,
        help="CMake generator (e.g., Ninja, Unix Makefiles)",
    )

    parser.add_argument(
        "--tag",
        default="",
        help="Source tag (e.g., alcy_00013) to download LLVM from alcy fork, defaults to use the current tag of `src_dir`",
    )

    parser.add_argument(
        "--download-dir",
        default=default_llvm_download_dir,
        help="Directory to store downloaded archives",
    )

    parser.add_argument(
        "--disable-cache-llvm",
        action="store_true",
        help="Not to use preinstalled LLVM if available",
    )

    parser.add_argument(
        "--disable-download-llvm",
        action="store_true",
        help="Not to download prebuilt LLVM from GitHub releases if available",
    )

    parser.add_argument(
        "--disable-build-llvm",
        action="store_true",
        help="Not to build LLVM if no caches or downloads available",
    )

    parser.add_argument(
        "--libcxx",
        action="store_true",
        help="Use libc++ for LLVM build",
    )

    args = parser.parse_args()

    if args.tag is None or args.tag == "":
        args.tag = current_tag(src_dir=args.src_dir)
        if args.tag is None or args.tag == "":
            print("Failed to fetch LLVM tag")
            return -1

    if not os.path.isdir(script_dir):
        print(f"Script directory not found: {script_dir}")
        return -2
    if not os.path.isdir(args.src_dir):
        print(f"Source directory not found: {args.src_dir}")
        return -3
    if not os.path.isfile(args.configure_script):
        print(f"Configure script not found: {args.configure_script}")
        return -4

    include_dir = os.path.join(args.install_dir, "include")
    lib_dir = os.path.join(args.install_dir, "lib")

    enable_cache_llvm = not args.disable_cache_llvm
    enable_download_llvm = not args.disable_download_llvm
    enable_build_llvm = not args.disable_build_llvm
    lower_type = args.type.lower()

    for arg, value in vars(args).items():
        print(f"{arg}: {value}")

    # Check tag cache file
    cached_tag = None
    if os.path.isfile(args.tag_cache_file):
        try:
            with open(args.tag_cache_file, "r") as f:
                cached_tag = f.read().strip()
        except Exception:
            cached_tag = None

    tag_matches = cached_tag == args.tag if cached_tag is not None else False

    # Skip setup only if cached tag matches target tag AND preinstalled files exist
    if (
        tag_matches
        and os.path.isdir(include_dir)
        and os.path.isdir(lib_dir)
        and enable_cache_llvm
    ):
        print(
            f"Preinstalled LLVM with matching tag found in {args.install_dir}; skipped operation."
        )
        return 0

    if not tag_matches and cached_tag is not None:
        print(
            f"Tag mismatch detected (cached: '{cached_tag}', requested: '{args.tag}'). Re-setting up LLVM..."
        )

    url = download_llvm.release_url(
        tag=args.tag, triple=args.triple, build_type=lower_type
    )
    exists = download_llvm.check_release_exists(url)
    if exists and enable_download_llvm:
        print(f"Found prebuilt binary in {url}")
        res = download_llvm.download_and_extract(
            tag=args.tag,
            triple=args.triple,
            build_type=lower_type,
            download_dir=args.download_dir,
            install_dir=args.install_dir,
        )
        if res == 0:
            write_tag_cache(args.tag, args.tag_cache_file)
        return res
    elif enable_build_llvm:
        res = build_llvm.build_llvm(
            build_type=args.type,
            configure_script=args.configure_script,
            llvm_src_dir=args.src_dir,
            llvm_build_dir=args.build_dir,
            llvm_install_dir=args.install_dir,
            compiler_launcher=args.compiler_launcher,
            c_compiler=args.cc,
            cxx_compiler=args.cxx,
            generator=args.generator,
            triple=args.triple,
            assertions=("ON" if args.type == "debug" else "OFF"),
            libcxx=("ON" if args.libcxx else "OFF"),
        )
        if res == 0:
            write_tag_cache(args.tag, args.tag_cache_file)
        return res
    else:
        print("Failed to setup LLVM")
        return -5


if __name__ == "__main__":
    sys.exit(main())

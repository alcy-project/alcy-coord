#!/usr/bin/env bash

# Copyright 2026 pugur
# This source code is licensed under the Apache License, Version 2.0 with LLVM
# Exceptions which can be found in the LICENSE file.

set -e

target=${1:-"default"}
mode=${2:-"debug"}
build_subdir=${3:-"build"}

script_dir="$(cd $(dirname $0) && pwd)"
source "$script_dir/env.sh"

build_dir="$out_dir/$build_subdir"

if [[ $mode == "debug" ]]; then
  is_debug="true"
else
  is_debug="false"
fi

gn gen $build_dir --args="is_debug=$is_debug use_lld=true"
gn check $build_dir "//src/*"
ninja -C $build_dir -t compdb > compile_commands.json
ninja -C $build_dir $target

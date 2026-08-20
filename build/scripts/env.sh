#!/bin/bash

set -e

script_dir="$(cd $(dirname $0) && pwd)"
root_dir="$(cd $script_dir/../.. && pwd)"
out_dir="$root_dir/out"

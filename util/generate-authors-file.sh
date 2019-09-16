#!/usr/bin/env bash

# Generate an AUTHORS file based on the output of git shortlog. Uses ABC
# order, strips out leading spaces and numbers, then filters out specific
# authors.

set -euo pipefail

git rev-parse --git-dir 1>/dev/null # exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

readonly authors_file="${git_root_dir}/AUTHORS"

git shortlog --summary --email \
  | perl -spe 's/^\s+\d+\s+//' \
  | sed -e '/^CommitSyncScript.*$/d' \
  > "${authors_file}"

echo "created/updated authors file: ${authors_file}"
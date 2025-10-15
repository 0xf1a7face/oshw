#!/bin/sh

set -e

run() {
  echo ">" "$@"
  "$@"
}

run rm -- *.a *.so main || true

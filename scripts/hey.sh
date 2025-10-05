#!/usr/bin/env bash
set -euo pipefail

URL=${1:-http://localhost:8000/data}
RATE=${RATE:-1000}
DURATION=${DURATION:-60s}
CONCURRENCY=${CONCURRENCY:-200}

if ! command -v hey >/dev/null 2>&1; then
  echo "hey not found. Install: go install github.com/rakyll/hey@latest or brew install hey" >&2
  exit 1
fi

echo "Running hey: ${RATE} rps for ${DURATION} at ${URL}"
hey -z ${DURATION} -q ${RATE} -c ${CONCURRENCY} ${URL}


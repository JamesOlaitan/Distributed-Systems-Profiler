# Contributing

- Use Python 3.10+ and C++17.
- Run `pytest -q` locally before PRs.
- Keep label cardinality low; avoid per-user labels.
- Prefer histograms for latency; update Grafana queries if buckets change.
- Pin dependencies where possible; use slim Docker images.

## Dev workflow
- `docker compose up --build -d` to run stack.
- `bash scripts/hey.sh` or `k6 run scripts/k6.js` for load.
- C++: `cmake -S src/profiler_engine -B build/profiler_engine && cmake --build build/profiler_engine`.

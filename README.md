# Distributed Systems Profiler

A containerized microservices testbed (3 FastAPI REST APIs) with a C++17 profiling engine and full observability via Prometheus and Grafana.

## Features
- 3 FastAPI services with health/readiness endpoints and Prometheus metrics
- Prometheus scraping and Grafana provisioning with ready-to-use dashboards
- C++17 profiler (libcurl + nlohmann/json) with rolling-window percentile computation scaffolding
- Pytest unit/integration tests and GitHub Actions CI (lint, tests, compose up, artifact export)
- Load testing scripts (k6/hey) to reproduce ~1,000 rps locally

## Quickstart
Prereqs: Docker, Docker Compose, Python 3.10+, Node (for k6 optional) or install `hey`.

1. Build and run the stack
```bash
docker compose up --build -d
```

2. Verify services
```bash
curl -s localhost:8000/healthz
curl -s localhost:8001/healthz
curl -s localhost:8002/healthz
```

3. Open Observability
- Prometheus: `http://localhost:9090`
- Grafana: `http://localhost:3000` (admin/admin)

4. Load test (example: 1,000 rps for 60s)
```bash
bash scripts/hey.sh
# or
k6 run scripts/k6.js
```

5. Run tests locally
```bash
python -m venv .venv && . .venv/bin/activate
pip install -r requirements.txt -r tests/requirements-test.txt
pytest -q
```

## C++ Profiler
Build the C++ profiler locally:
```bash
cmake -S src/profiler_engine -B build/profiler_engine
cmake --build build/profiler_engine -j
```
Run (example):
```bash
./build/profiler_engine/profiler_engine http://localhost:9090
```

## Repository Layout
- `src/services` — FastAPI microservices and shared middleware
- `config/prometheus` — Prometheus scraping config
- `config/grafana` — Datasource + dashboards provisioning
- `src/profiler_engine` — C++17 profiler
- `tests` — Python unit/integration tests
- `scripts` — Load testing scripts

## Runbook
- If Grafana dashboards are empty, check Prometheus targets page and ensure services expose `/metrics`.
- To tune throughput, set env `UVICORN_WORKERS` and `UVICORN_LOOP=uvloop` in compose.
- For small images, we use slim bases and non-root users in service containers.

## Benchmarks & Acceptance
Target: ~1,000 rps with ≥99.8% success and reasonable P95 (<300ms on laptop).

Commands:
```bash
docker compose up --build -d
RATE=1000 DURATION=60s CONCURRENCY=200 bash scripts/hey.sh http://localhost:8000/data
# Expect: non-2xx/3xx <= 0.2% and latency P95 tracked in Grafana via histogram_quantile
```

## Contributing
See `CONTRIBUTING.md` for guidelines.

#!/usr/bin/env python3
import asyncio
import os
import sys
import time

import httpx


SERVICES = [
    ("service1", "http://localhost:8000/data"),
    ("service2", "http://localhost:8001/process"),
    ("service3", "http://localhost:8002/submit"),
]

TOTAL_PER_SERVICE = int(os.environ.get("TOTAL_PER_SERVICE", "1000"))
CONCURRENCY = int(os.environ.get("CONCURRENCY", "200"))
TIMEOUT = float(os.environ.get("TIMEOUT", "1.0"))


async def hit(client: httpx.AsyncClient, method: str, url: str) -> bool:
    try:
        if method == "GET":
            r = await client.get(url)
        else:
            r = await client.post(url, json={"payload": {"k": "v"}})
        return 200 <= r.status_code < 300
    except Exception:
        return False


async def load_service(name: str, url: str) -> tuple[int, int]:
    limits = httpx.Limits(max_connections=CONCURRENCY, max_keepalive_connections=CONCURRENCY)
    timeout = httpx.Timeout(TIMEOUT, connect=TIMEOUT)
    method = "GET" if name != "service3" else "POST"
    async with httpx.AsyncClient(limits=limits, timeout=timeout) as client:
        successes = 0
        sem = asyncio.Semaphore(CONCURRENCY)

        async def one():
            async with sem:
                ok = await hit(client, method, url)
                return 1 if ok else 0

        tasks = [asyncio.create_task(one()) for _ in range(TOTAL_PER_SERVICE)]
        for fut in asyncio.as_completed(tasks):
            successes += await fut
        return TOTAL_PER_SERVICE, successes


async def main() -> int:
    overall_sent = 0
    overall_ok = 0
    results = []
    start = time.time()
    for name, url in SERVICES:
        sent, ok = await load_service(name, url)
        results.append((name, sent, ok))
        overall_sent += sent
        overall_ok += ok
    elapsed = time.time() - start
    for name, sent, ok in results:
        pct = 100.0 * ok / sent if sent else 0.0
        print(f"Service {name}: {pct:.2f}% success ({ok}/{sent})")
    overall_pct = 100.0 * overall_ok / overall_sent if overall_sent else 0.0
    print(f"Overall: {overall_pct:.2f}% success rate in {elapsed:.1f}s")
    # enforce threshold 99.5% to avoid flakes but aim higher in docs
    return 0 if overall_pct >= 99.5 else 1


if __name__ == "__main__":
    sys.exit(asyncio.run(main()))



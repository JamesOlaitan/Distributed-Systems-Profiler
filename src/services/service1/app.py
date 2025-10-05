from fastapi import FastAPI, HTTPException
from pydantic import BaseModel, Field
import httpx
from starlette.responses import Response
from prometheus_client import generate_latest, CONTENT_TYPE_LATEST
from services.common.metrics_middleware import MetricsMiddleware
import random
import asyncio
import uvicorn

app = FastAPI()

# Adds Metrics Middleware with service name 'service1'
app.add_middleware(MetricsMiddleware, service_name='service1')


@app.get("/healthz")
async def health() -> dict:
    return {"status": "ok"}


@app.get("/readyz")
async def ready() -> dict:
    return {"ready": True}


@app.get("/data")
async def get_data() -> dict:
    """
    Handle GET requests to the /data endpoint.

    Simulates data retrieval with a chance of failure.

    Returns:
        dict: A message indicating successful data retrieval.

    Raises:
        HTTPException: If an error occurs during data retrieval.
    """
    # Simulates data processing with a random chance of failure
    if random.random() < 0.1:  # 10% chance of error
        raise HTTPException(status_code=500, detail="Internal Server Error")

    return {"message": "Data retrieved successfully from Service 1"}


@app.get("/fanout")
async def fanout() -> dict:
    """
    Calls service2 and service3 to validate service discovery and measure success rate.
    """
    timeouts = httpx.Timeout(1.0, connect=0.5)
    async with httpx.AsyncClient(timeout=timeouts) as client:
        async def call_service2():
            for attempt in range(2):
                try:
                    r = await client.get("http://service2:8001/process")
                    r.raise_for_status()
                    return True
                except Exception:
                    if attempt == 1:
                        return False
        async def call_service3():
            for attempt in range(2):
                try:
                    r = await client.post("http://service3:8002/submit", json={"payload": {"k": "v"}})
                    r.raise_for_status()
                    return True
                except Exception:
                    if attempt == 1:
                        return False
        ok2, ok3 = await asyncio.gather(call_service2(), call_service3())
    success = 1 if (ok2 and ok3) else 0
    return {"service2": ok2, "service3": ok3, "success": success}


@app.get("/metrics")
async def metrics():
    """
    Expose Prometheus metrics at the /metrics endpoint.

    Returns:
        Response: Prometheus metrics in text format.
    """
    return Response(generate_latest(), media_type=CONTENT_TYPE_LATEST)

if __name__ == '__main__':
    uvicorn.run("app:app", host="127.0.0.1", port=8000, reload=True)
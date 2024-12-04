from fastapi import FastAPI, HTTPException
from starlette.responses import Response
from prometheus_client import generate_latest, CONTENT_TYPE_LATEST
from common.metrics_middleware import MetricsMiddleware
import random
import asyncio

app = FastAPI()

# Adds Metrics Middleware with service name 'service1'
app.add_middleware(MetricsMiddleware, service_name='service1')


@app.get("/data")
async def get_data():
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


@app.get("/metrics")
async def metrics():
    """
    Expose Prometheus metrics at the /metrics endpoint.

    Returns:
        Response: Prometheus metrics in text format.
    """
    return Response(generate_latest(), media_type=CONTENT_TYPE_LATEST)
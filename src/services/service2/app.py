from fastapi import FastAPI, HTTPException
from starlette.responses import Response
from prometheus_client import generate_latest, CONTENT_TYPE_LATEST
from services.common.metrics_middleware import MetricsMiddleware
import random
import asyncio
import uvicorn

app = FastAPI()

# Adds Metrics Middleware with service name 'service2'
app.add_middleware(MetricsMiddleware, service_name='service2')


@app.get("/process")
async def process_data():
    """
    Handles GET requests to the /process endpoint.

    Simulates data processing with variable latency.

    Returns:
        dict: A message indicating successful data processing.

    Raises:
        HTTPException: If an error occurs during data processing.
    """
    # Simulates variable processing time
    processing_time = random.uniform(0.1, 0.5)  # 100ms to 500ms
    await asyncio.sleep(processing_time)

    # Simulates error occurrence
    if random.random() < 0.05:  # 5% chance of error
        raise HTTPException(status_code=400, detail="Bad Request")

    return {"message": "Data processed successfully by Service 2"}


@app.get("/metrics")
async def metrics():
    """
    Expose Prometheus metrics at the /metrics endpoint.

    Returns:
        Response: Prometheus metrics in text format.
    """
    return Response(generate_latest(), media_type=CONTENT_TYPE_LATEST)

if __name__ == '__main__':
    uvicorn.run("app:app", host="127.0.0.1", port=8001, reload=True)
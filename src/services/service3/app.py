from fastapi import FastAPI
from starlette.responses import Response
from prometheus_client import generate_latest, CONTENT_TYPE_LATEST
from common.metrics_middleware import MetricsMiddleware

app = FastAPI()

# Add Metrics Middleware with service name 'service3'
app.add_middleware(MetricsMiddleware, service_name='service3')


@app.post("/submit")
async def submit_data(data: dict):
    """
    Handle POST requests to the /submit endpoint.

    Simulates data submission.

    Args:
        data (dict): The data payload to submit.

    Returns:
        dict: A message indicating successful data submission.
    """
    # Simulates data submission processing
    return {"message": "Data submitted successfully to Service 3"}


@app.get("/metrics")
async def metrics():
    """
    Expose Prometheus metrics at the /metrics endpoint.

    Returns:
        Response: Prometheus metrics in text format.
    """
    return Response(generate_latest(), media_type=CONTENT_TYPE_LATEST)
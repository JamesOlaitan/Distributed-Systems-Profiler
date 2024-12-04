from prometheus_client import Counter, Histogram
from starlette.middleware.base import BaseHTTPMiddleware
from starlette.responses import Response
from fastapi import Request


# Prometheus metrics
REQUEST_COUNT = Counter(
    'http_requests_total',
    'Total number of HTTP requests',
    ['service', 'method', 'endpoint']
)
REQUEST_LATENCY = Histogram(
    'http_request_latency_seconds',
    'Latency of HTTP requests in seconds',
    ['service', 'endpoint']
)
ERROR_COUNT = Counter(
    'http_errors_total',
    'Total number of HTTP errors',
    ['service', 'method', 'endpoint']
)


class MetricsMiddleware(BaseHTTPMiddleware):
    """
    Middleware to collect metrics for each incoming HTTP request.

    This middleware captures the request count, latency, and error count
    for each endpoint and method.
    """

    def __init__(self, app, service_name: str):
        """
        Initialize the MetricsMiddleware.

        Args:
            app: The FastAPI application instance.
            service_name (str): The name of the service.
        """
        super().__init__(app)
        self.service_name = service_name

    async def dispatch(self, request: Request, call_next):
        """
        Process the incoming request and collect metrics.

        Args:
            request (Request): The incoming HTTP request.
            call_next: The next middleware or route handler.

        Returns:
            Response: The HTTP response.
        """
        method = request.method
        endpoint = request.url.path

        REQUEST_COUNT.labels(
            service=self.service_name,
            method=method,
            endpoint=endpoint
        ).inc()

        with REQUEST_LATENCY.labels(
            service=self.service_name,
            endpoint=endpoint
        ).time():
            try:
                response = await call_next(request)
                if response.status_code >= 400:
                    ERROR_COUNT.labels(
                        service=self.service_name,
                        method=method,
                        endpoint=endpoint
                    ).inc()
                return response
            except Exception as exc:
                ERROR_COUNT.labels(
                    service=self.service_name,
                    method=method,
                    endpoint=endpoint
                ).inc()
                raise exc
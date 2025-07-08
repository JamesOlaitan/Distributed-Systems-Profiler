import sys
from pathlib import Path
from unittest.mock import patch

import pytest
from fastapi.testclient import TestClient
from prometheus_client import CONTENT_TYPE_LATEST

# need to add the src directory to the path so "services" can be imported
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "src"))

from services.service1.app import app

client = TestClient(app)


def test_data_success():
    """/data should return 200 when no error is triggered."""
    with patch("services.service1.app.random.random", return_value=0.5):
        response = client.get("/data")
    assert response.status_code == 200
    assert response.json() == {"message": "Data retrieved successfully from Service 1"}


def test_data_error():
    """/data should return 500 when an error is triggered."""
    with patch("services.service1.app.random.random", return_value=0.05):
        response = client.get("/data")
    assert response.status_code == 500
    assert response.json()["detail"] == "Internal Server Error"


def test_metrics_endpoint():
    """/metrics should expose Prometheus metrics."""
    response = client.get("/metrics")
    assert response.status_code == 200
    assert response.headers["content-type"] == CONTENT_TYPE_LATEST

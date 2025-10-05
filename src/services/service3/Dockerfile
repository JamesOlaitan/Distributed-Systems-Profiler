FROM python:3.11-slim

# Sets the working directory
WORKDIR /app

# Copies the requirements file and installs dependencies
COPY requirements.txt ./
RUN pip install --no-cache-dir -r requirements.txt

# Copies the application code
COPY src/services/service3/app.py ./app.py
COPY src/services/common/ /app/services/common/
RUN adduser --disabled-password --gecos '' appuser && chown -R appuser:appuser /app
USER appuser

# Exposes Service 3 port
EXPOSE 8002

# Command to run the application
ENV PYTHONUNBUFFERED=1
CMD ["uvicorn", "app:app", "--host", "0.0.0.0", "--port", "8002"]
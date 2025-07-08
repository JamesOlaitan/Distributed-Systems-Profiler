FROM python:3.9-slim

# Sets the working directory
WORKDIR /app

# Copies the requirements file and installs dependencies
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Copies the application code
COPY app.py .
COPY ../common/ /app/common/

# Exposes Service 2 port
EXPOSE 8001

# Command to run the application
CMD ["uvicorn", "app:app", "--host", "0.0.0.0", "--port", "8001"]
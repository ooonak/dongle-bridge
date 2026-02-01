#!/bin/bash
set -euo pipefail

# Build Docker image
IMAGE_NAME="zephyr-slim-ci-test"
DOCKERFILE="Dockerfile"

echo "Building Docker image ${IMAGE_NAME}..."
docker build -t "$IMAGE_NAME" -f "$DOCKERFILE" .

# Run container and execute CI commands
echo "Running CI commands inside container..."

docker run --rm -v "$PWD":/work -w /work "$IMAGE_NAME" /bin/bash -c "
set -euo pipefail

echo 'Initializing west...'
west init -l .
west update

echo 'Building native_sim...'
west build -p -b native_sim

echo 'Quick run native_sim (5s timeout)...'
timeout 5 ./build/zephyr/zephyr.exe || echo 'Timeout reached'

echo 'Running native_sim tests (Twister)...'
west twister -p native_sim

echo 'Building nRF52840...'
west build -p -b nrf52840dongle
"

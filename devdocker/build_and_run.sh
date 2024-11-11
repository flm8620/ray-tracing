#!/bin/bash
set -e # exit if fail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SCRIPT_FOLDER="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ -z "$HTTP_PROXY" ]; then
    echo "Error: Proxy variables are not set."
    exit 1
fi

cd $SCRIPT_FOLDER

bash download_pkgs.sh

USER_ID=$(id -u)
GROUP_ID=$(id -g)
USER_NAME=$(id -un)

# Define the tag for your image
IMAGE_NAME="raytracing-dev"
IMAGE_TAG="v2"
CONTAINER_NAME="$USER_NAME-$IMAGE_NAME-$IMAGE_TAG"

# Build the Docker image
echo "Building Docker image $IMAGE_NAME:$IMAGE_TAG..."

# "DOCKER_BUILDKIT=0": buildkit do not support nvidia runtime during docker build
# https://github.com/NVIDIA/nvidia-container-toolkit/issues/122
# https://stackoverflow.com/questions/57389725/building-docker-image-with-cuda-runtime
DOCKER_BUILDKIT=0 docker build \
    -t $IMAGE_NAME:$IMAGE_TAG \
    --build-arg HTTPS_PROXY=$HTTPS_PROXY \
    --build-arg HTTP_PROXY=$HTTP_PROXY \
    --build-arg ALL_PROXY=$ALL_PROXY \
    --build-arg NO_PROXY=$NO_PROXY \
    --network host \
    --build-arg USER_ID=${USER_ID} \
    --build-arg GROUP_ID=${GROUP_ID} \
    --build-arg USER_NAME=${USER_NAME} \
    docker_build

echo "Docker image built successfully."

# Check if the container is already running
if [ $(docker ps -aq -f name=^$CONTAINER_NAME$) ]; then
    echo "A container named '$CONTAINER_NAME' already exists."
    echo -n "Do you want to stop and remove it? (y/N): "
    read answer
    answer=${answer:-n} # Default answer is 'n' if no input is entered

    if [ "$answer" = "y" ]; then
        echo "Stopping and removing existing container..."
        docker stop $CONTAINER_NAME
        docker rm $CONTAINER_NAME
    else
        echo "Leaving the existing container unchanged."
        exit 0
    fi
fi

echo "Running Docker container named $CONTAINER_NAME..."

docker run -dit \
    --gpus all \
    --network=host \
    -e HTTPS_PROXY=$HTTPS_PROXY \
    -e HTTP_PROXY=$HTTP_PROXY \
    -e ALL_PROXY=$ALL_PROXY \
    -e NO_PROXY=$NO_PROXY \
    --name $CONTAINER_NAME \
    -v $HOME:/home/$USER/host_home/ \
    $IMAGE_NAME:$IMAGE_TAG

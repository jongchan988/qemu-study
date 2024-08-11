#!/bin/bash

# 컨테이너 이름
CONTAINER_NAME="qemu"

# 로컬 소스 디렉터리
SOURCE_DIR="./source/."

# 컨테이너 내의 목적지 경로
DESTINATION_PATH="/jcstudy/qemu/"

docker cp "$SOURCE_DIR" "$CONTAINER_NAME:$DESTINATION_PATH"

echo "finish"
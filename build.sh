#!/bin/bash

# 컨테이너 이름
CONTAINER_NAME="qemu-avr32"

# 로컬 소스 디렉터리
SOURCE_DIR="./source-avr32/."

# 컨테이너 내의 목적지 경로
DESTINATION_PATH="/jcstudy/qemu-avr32/"

# cp to container
docker cp "$SOURCE_DIR" "$CONTAINER_NAME:$DESTINATION_PATH"
# cmd
DOCKER_CMD="./configure --target-list=jc1904-softmmu"
# exec
docker exec -it "$CONTAINER_NAME" bash -c "$DOCKER_CMD"
# make
docker exec -it "$CONTAINER_NAME" bash -c "make -j 16"


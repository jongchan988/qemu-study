FROM ubuntu:latest

RUN mkdir -p /jcstudy
WORKDIR /jcstudy

# 필요한 의존성 설치
RUN apt-get update && apt-get install -y \
    git \
    libglib2.0-dev \
    libfdt-dev \
    libpixman-1-dev \
    zlib1g-dev \
    ninja-build \
    build-essential \
    python3 \
    python3-venv \
    python3-pip \
    flex \
    bison

RUN git clone https://github.com/qemu/qemu.git

# 작업 디렉토리 설정
WORKDIR /jcstudy/qemu

# configure 스크립트 실행
RUN ./configure
RUN make
 --target-list=avr32-softmmu
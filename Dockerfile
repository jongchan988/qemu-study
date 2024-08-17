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
    python3-colorama \
    flex \
    bison \
    vim

RUN git clone https://github.com/flogosec/qemu-avr32.git

RUN git clone https://github.com/flogosec/qemu-avr32-testing.git

# 작업 디렉토리 설정
WORKDIR /jcstudy/qemu-avr32

# avr32-gnu-toolchain
COPY avr32-gnu-toolchain /jcstudy/avr32-gnu-toolchain
COPY qemu-avr32-testing /jcstudy/qemu-avr32-testing

RUN /bin/bash /jcstudy/avr32-gnu-toolchain/install.sh

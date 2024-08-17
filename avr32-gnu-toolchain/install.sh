#!/bin/bash

# 다운로드할 파일 및 설치 경로 설정
TOOLCHAIN_URL="https://ww1.microchip.com/downloads/en/DeviceDoc/avr32-gnu-toolchain-3.4.2.435-linux.any.x86.tar.gz"
HEADERS_URL="https://ww1.microchip.com/downloads/en/DeviceDoc/atmel-headers-6.1.3.1475.zip"
DOWNLOAD_DIR="/jcstudy/avr32-gnu-toolchain"
INSTALL_DIR="/usr/local/avr32-gnu-toolchain-linux_x86"

# 필요한 패키지 설치
echo "패키지 설치 중..."
apt-get update
apt-get install -y build-essential unzip wget

# 툴체인 다운로드
echo "AVR32 툴체인 다운로드 중..."
wget -O "$DOWNLOAD_DIR/avr32-gnu-toolchain.tar.gz" $TOOLCHAIN_URL

echo "Atmel 헤더 파일 다운로드 중..."
wget -O "$DOWNLOAD_DIR/atmel-headers.zip" $HEADERS_URL

# 툴체인 설치
echo "툴체인 압축 해제 및 설치 중..."
tar xvfz "$DOWNLOAD_DIR/avr32-gnu-toolchain.tar.gz" -C $DOWNLOAD_DIR
mv "$DOWNLOAD_DIR/avr32-gnu-toolchain-linux_x86" $INSTALL_DIR

# 헤더 파일 설치
echo "Atmel 헤더 파일 설치 중..."
unzip "$DOWNLOAD_DIR/atmel-headers.zip" -d "$DOWNLOAD_DIR/atmel-headers"
mv "$DOWNLOAD_DIR/atmel-headers/*" "$INSTALL_DIR/avr32/include"
rm -rf "$DOWNLOAD_DIR/atmel-headers"

# 심볼릭 링크 생성
echo "심볼릭 링크 생성 중..."
ln -s $INSTALL_DIR/bin/avr32* /usr/local/bin

# 환경 변수 설정
echo "환경 변수 설정 중..."
echo "export PATH=\$PATH:$INSTALL_DIR/bin" >> ~/.bashrc
source ~/.bashrc

# 설치 확인
echo "설치 확인 중..."
avr32-gcc -v && echo "AVR32 툴체인 설치가 완료되었습니다." || echo "설치에 실패했습니다."

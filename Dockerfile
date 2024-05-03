FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    cmake \
    libcurl4-openssl-dev \
    libfftw3-dev \
    ffmpeg 

COPY . /app
WORKDIR /app

RUN cmake . -DENABLE_PROFILING=ON
RUN make -j9

CMD ["./vibra", "-R", "--file", "tests/sample.mp3"]

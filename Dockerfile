FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    jq \
    gcc \
    cmake \
    ffmpeg \
    build-essential \
    libcurl4-openssl-dev \
    libfftw3-dev

COPY . /app
WORKDIR /app

RUN cmake . -DENABLE_PROFILING=ON
RUN make -j9

CMD ./vibra --recognize --file tests/sample.mp3 | jq \
    # && gprof ./vibra gmon.out 
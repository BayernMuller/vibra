FROM emscripten/emsdk

# Activate Emscripten SDK
RUN emsdk install latest
RUN emsdk activate latest

ENV FFTW_VERSION=3.3.10
ENV FFTW3_PATH=/usr/local
RUN wget http://www.fftw.org/fftw-${FFTW_VERSION}.tar.gz && \
    tar -xvf fftw-${FFTW_VERSION}.tar.gz && \
    cd fftw-${FFTW_VERSION} && \
    emconfigure ./configure --disable-fortran --prefix=${FFTW3_PATH} && \
    emmake make -j9 && \
    emmake make install

WORKDIR /app
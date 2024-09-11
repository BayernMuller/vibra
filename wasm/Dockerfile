FROM emscripten/emsdk

# Activate Emscripten SDK
RUN emsdk install latest
RUN emsdk activate latest

# Build FFTW with Emscripten
RUN FFTW_VERSION=3.3.10 && \
    wget http://www.fftw.org/fftw-${FFTW_VERSION}.tar.gz && \
    tar -xvf fftw-${FFTW_VERSION}.tar.gz && \
    cd fftw-${FFTW_VERSION} && \
    emconfigure ./configure --disable-fortran && \
    emmake make -j9 && \
    emmake make install

COPY ../ /app
WORKDIR /app

ENV FFTW3_PATH=/usr/local

CMD cd /app/wasm && \
    ls $FFTW3_PATH/include && \
    ls $FFTW3_PATH/lib && \
    emmake make -f Makefile.wasm -j9 && \
    ls -l build

import numpy as np

def hanning_generator():
    HANNING_MATRIX = np.hanning(2050)[1:-1]
    for i in HANNING_MATRIX:
        print("{:.15f},".format(i))
        
def test_fft():
    input = np.array([1,2,3,4,5,6,7,8,9,10,0])
    fft_results = np.fft.rfft(input)
    print('input:', input)
    print('real:', fft_results.real)
    print('imag:', fft_results.imag)

    print('real^2', np.square(fft_results.real))
    print('imag^2', np.square(fft_results.imag))

    print('real^2 + imag^2', np.square(fft_results.real) + np.square(fft_results.imag))
    print('(real^2 + imag^2) / (1 << 17)', (np.square(fft_results.real) + np.square(fft_results.imag)) / (1 << 17))
    print('max((real^2 + imag^2) / (1 << 17), 1e-10)', np.maximum((np.square(fft_results.real) + np.square(fft_results.imag)) / (1 << 17), 1e-10))


test_fft()
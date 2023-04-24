#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <vector>

template <typename T>
class RingBuffer
{
public:
    RingBuffer(std::size_t size);
    ~RingBuffer();

    void Append(const T& value);

    std::uint32_t Size() const { return mSize; }
    std::uint32_t NumWritten() const { return mNumWritten; }
    std::uint32_t Position() const { return mPosition; }

    T& operator[](std::uint32_t index)
    {
        return mVec[index];
    }

private:
    std::uint32_t mSize;
    std::uint32_t mNumWritten;
    std::uint32_t mPosition;
    std::vector<T> mVec;
};

template <typename T>
RingBuffer<T>::RingBuffer(std::size_t size)
    : mVec(size)
    , mSize(size)
    , mNumWritten(0)
    , mPosition(0)
{
}

template <typename T>
RingBuffer<T>::~RingBuffer()
{
}

template <typename T>
void RingBuffer<T>::Append(const T& value)
{
    mVec[mPosition] = value;
    mPosition = (mPosition + 1) % mSize;
    mNumWritten++;
}

#endif // RING_BUFFER_H
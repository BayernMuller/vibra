#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <vector>

template <typename T>
class RingBuffer : private std::vector<T>
{
public:
    RingBuffer(std::size_t size, T&& defaultValue = T());
    virtual ~RingBuffer();

    void Append(const T& value);
    std::uint32_t Size() const { return std::vector<T>::size(); }
    std::uint32_t& NumWritten() { return mNumWritten; }
    std::uint32_t& Position() { return mPosition; }

    T& operator[](std::size_t index) { return std::vector<T>::operator[](index); }
    
    typename std::vector<T>::iterator begin() { return std::vector<T>::begin(); }
    typename std::vector<T>::iterator end() { return std::vector<T>::end(); }

private:
    std::uint32_t mNumWritten;
    std::uint32_t mPosition;
};

template <typename T>
RingBuffer<T>::RingBuffer(std::size_t size, T&& defaultValue)
    : std::vector<T>(size, defaultValue)
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
    this->operator[](mPosition) = value;
    mPosition = (mPosition + 1) % std::vector<T>::size();
    mNumWritten++;
}

#endif // RING_BUFFER_H
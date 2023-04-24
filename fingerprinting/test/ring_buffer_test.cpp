#include <iostream>
#include "../algorithm/ring_buffer.h"

int main(int argc, char* argv[])
{
    RingBuffer<int> ring_buffer(15);

    for (int i = 0; i < 20; i++)
    {
        ring_buffer.Append(i);
    }

    for (int i = 0; i < 15; i++)
    {
        std::cout << ring_buffer[i] << std::endl;
    }

    std::cout << "Size: " << ring_buffer.Size() << std::endl;
    std::cout << "NumWritten: " << ring_buffer.NumWritten() << std::endl;
    std::cout << "Position: " << ring_buffer.Position() << std::endl;

    return 0;
}
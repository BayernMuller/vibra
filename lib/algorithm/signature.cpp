#include "algorithm/signature.h"
#include <algorithm>
#include <sstream>
#include <string>
#include "utils/base64.h"
#include "utils/crc32.h"

Signature::Signature(std::uint32_t sampleRate, std::uint32_t numberOfSamples)
    : mSampleRate(sampleRate), mNumberOfSamples(numberOfSamples)
{
}

void Signature::Reset(std::uint32_t sampleRate, std::uint32_t numberOfSamples)
{
    mSampleRate = sampleRate;
    mNumberOfSamples = numberOfSamples;
    mFrequancyBandToPeaks.clear();
}

std::uint32_t Signature::SumOfPeaksLength() const
{
    std::uint32_t sum = 0;
    for (const auto &pair : mFrequancyBandToPeaks)
    {
        sum += pair.second.size();
    }
    return sum;
}

std::string Signature::GetBase64Uri() const
{
    RawSignatureHeader header;
    header.magic1 = 0xcafe2580;
    header.magic2 = 0x94119c00;
    header.shifted_sample_rate_id = 3 << 27;
    header.fixed_value = ((15 << 19) + 0x40000);
    header.number_samples_plus_divided_sample_rate =
        static_cast<std::uint32_t>(mNumberOfSamples + mSampleRate * 0.24);
    std::stringstream contents;
    for (const auto &pair : mFrequancyBandToPeaks)
    {
        const auto &band = pair.first;
        const auto &peaks = pair.second;

        std::stringstream peak_buf;
        std::size_t fft_pass_number = 0;

        for (const auto &peak : peaks)
        {
            if (peak.GetFFTPassNumber() - fft_pass_number >= 255)
            {
                peak_buf << "\xff";
                writeLittleEndian(peak_buf, peak.GetFFTPassNumber());
                fft_pass_number = peak.GetFFTPassNumber();
            }

            peak_buf << static_cast<char>(peak.GetFFTPassNumber() - fft_pass_number);
            writeLittleEndian(peak_buf, peak.GetPeakMagnitude(), 2);
            writeLittleEndian(peak_buf, peak.GetCorrectedPeakFrequencyBin(), 2);

            fft_pass_number = peak.GetFFTPassNumber();
        }

        writeLittleEndian(contents, 0x60030040u + static_cast<std::uint32_t>(band));
        writeLittleEndian(contents, static_cast<std::uint32_t>(peak_buf.str().size()));
        contents << peak_buf.str();

        for (std::size_t i = 0; i < (-peak_buf.str().size() % 4); ++i)
            contents << '\0';
    }

    header.size_minus_header = contents.str().size() + 8;

    std::stringstream header_buf;
    header_buf.write(reinterpret_cast<const char *>(&header), sizeof(header));

    writeLittleEndian(header_buf, 0x40000000u);
    writeLittleEndian(header_buf, static_cast<std::uint32_t>(contents.str().size()) + 8);

    header_buf << contents.str();

    const auto &header_buf_str = header_buf.str();
    header.crc32 = crc32::crc32(header_buf_str.c_str() + 8, header_buf_str.size() - 8) & 0xffffffff;

    header_buf.seekp(0);
    header_buf.write(reinterpret_cast<const char *>(&header), sizeof(header));

    std::string header_string = header_buf.str();

    std::string base64Uri;
    base64Uri += "data:audio/vnd.shazam.sig;base64,";
    base64Uri += base64::encode(header_string.c_str(), header_string.size());
    return base64Uri;
}

Signature::~Signature()
{
}

#ifndef LIBHTTP_STREAM_H
#define LIBHTTP_STREAM_H

#include <functional>
#include <fstream>
#include <string>

namespace http
{
    using DownloadProgressCallback = std::function<void(size_t current, size_t total)>;

    class StreamDownload
    {
    public:
        StreamDownload() = default;

        void set_chunk_size(size_t size) { chunk_size_ = size; }
        size_t get_chunk_size() const { return chunk_size_; }

        void set_progress_callback(const DownloadProgressCallback &callback) { progress_callback_ = callback; }

        template <typename ChunkProcessor>
        bool download_with_processor(const std::string &body, const std::string &content_length_header,
                                     const ChunkProcessor &processor)
        {
            size_t total = 0;
            try
            {
                total = content_length_header.empty() ? 0 : std::stoul(content_length_header);
            }
            catch (...)
            {
                total = body.length();
            }

            size_t bytes_processed = 0;
            for (size_t offset = 0; offset < body.length(); offset += chunk_size_)
            {
                size_t chunk_len = std::min(chunk_size_, body.length() - offset);
                const char *chunk_data = body.data() + offset;

                if (!processor(chunk_data, chunk_len))
                {
                    return false;
                }

                bytes_processed += chunk_len;
                if (progress_callback_ && total > 0)
                {
                    progress_callback_(bytes_processed, total);
                }
            }

            if (progress_callback_ && total > 0)
            {
                progress_callback_(bytes_processed, total);
            }

            return true;
        }

        bool download_to_file(const std::string &body, const std::string &filepath,
                              const std::string &content_length_header = "")
        {
            std::ofstream file(filepath, std::ios::binary | std::ios::trunc);
            if (!file.is_open())
            {
                return false;
            }

            bool success = download_with_processor(
                body, content_length_header, [&file](const char *data, size_t len)
                {
                file.write(data, static_cast<std::streamsize>(len));
                return file.good(); });

            file.close();
            return success && file.good();
        }

    private:
        size_t chunk_size_ = 8192;
        DownloadProgressCallback progress_callback_;
    };

} // namespace http

#endif

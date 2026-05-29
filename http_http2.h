#ifndef LIBHTTP_HTTP2_H
#define LIBHTTP_HTTP2_H

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <cstdint>

namespace http
{
    enum class HttpVersion
    {
        HTTP_1_1,
        HTTP_2
    };

    namespace detail
    {
        // HTTP/2 Frame Types
        enum class Http2FrameType : uint8_t
        {
            DATA = 0x0,
            HEADERS = 0x1,
            PRIORITY = 0x2,
            RST_STREAM = 0x3,
            SETTINGS = 0x4,
            PUSH_PROMISE = 0x5,
            PING = 0x6,
            GOAWAY = 0x7,
            WINDOW_UPDATE = 0x8,
            CONTINUATION = 0x9
        };

        // HTTP/2 Frame Flags
        struct Http2FrameFlags
        {
            bool end_stream = false;
            bool end_headers = false;
            bool ack = false;
            bool padded = false;
            bool priority = false;

            uint8_t to_byte() const
            {
                uint8_t flags = 0;
                if (end_stream) flags |= 0x01;
                if (end_headers) flags |= 0x04;
                if (ack) flags |= 0x01;
                if (padded) flags |= 0x08;
                if (priority) flags |= 0x20;
                return flags;
            }
        };

        // HTTP/2 Frame Header (9 bytes)
        struct Http2FrameHeader
        {
            uint32_t length;      // 24 bits
            Http2FrameType type;
            uint8_t flags;
            uint32_t stream_id;   // 31 bits (1 bit reserved)

            std::string serialize() const
            {
                std::string header(9, '\0');
                // Length (24 bits, big-endian)
                header[0] = static_cast<char>((length >> 16) & 0xFF);
                header[1] = static_cast<char>((length >> 8) & 0xFF);
                header[2] = static_cast<char>(length & 0xFF);
                // Type
                header[3] = static_cast<char>(type);
                // Flags
                header[4] = static_cast<char>(flags);
                // Stream ID (31 bits)
                uint32_t sid = stream_id & 0x7FFFFFFF;
                header[5] = static_cast<char>((sid >> 24) & 0xFF);
                header[6] = static_cast<char>((sid >> 16) & 0xFF);
                header[7] = static_cast<char>((sid >> 8) & 0xFF);
                header[8] = static_cast<char>(sid & 0xFF);
                return header;
            }
        };

        // HTTP/2 Stream representation
        class Http2Stream
        {
        public:
            enum class State
            {
                IDLE,
                OPEN,
                HALF_CLOSED_LOCAL,
                HALF_CLOSED_REMOTE,
                CLOSED
            };

            Http2Stream(uint32_t id) : stream_id(id), state(State::IDLE) {}

            uint32_t get_id() const { return stream_id; }
            State get_state() const { return state; }
            void set_state(State s) { state = s; }

            void append_data(const std::string &data) { body += data; }
            const std::string &get_body() const { return body; }
            void clear_body() { body.clear(); }

            void set_header(const std::string &key, const std::string &value)
            {
                response_headers[key] = value;
            }

            const std::map<std::string, std::string> &get_headers() const
            {
                return response_headers;
            }

        private:
            uint32_t stream_id;
            State state;
            std::string body;
            std::map<std::string, std::string> response_headers;
        };

        // HTTP/2 Connection Manager
        class Http2Connection
        {
        public:
            Http2Connection(int socket_fd, bool is_https = false)
                : sockfd(socket_fd), is_https(is_https), 
                  next_stream_id(1), remote_flow_window(65535),
                  local_flow_window(65535)
            {
                // Send HTTP/2 connection preface
                send_preface();
            }

            ~Http2Connection() = default;

            // Send HTTP/2 connection preface
            void send_preface()
            {
                const std::string preface = "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n";
                // In real implementation, this would be sent via SSL or plain socket
                // For now, we prepare it but actual sending is in request_once
            }

            // Encode request headers using HPACK (simplified)
            std::string encode_headers(const Headers &headers)
            {
                std::string encoded;
                for (const auto &h : headers)
                {
                    // Simplified header encoding (literal with incremental indexing)
                    // In production, use proper HPACK library
                    encoded += static_cast<char>(0x40); // Literal with incremental indexing
                    encoded += static_cast<char>(h.first.length());
                    encoded += h.first;
                    encoded += static_cast<char>(h.second.length());
                    encoded += h.second;
                }
                return encoded;
            }

            // Create and send a DATA frame
            std::string create_data_frame(uint32_t stream_id, const std::string &data)
            {
                Http2FrameHeader header;
                header.length = data.length();
                header.type = Http2FrameType::DATA;
                header.flags = 0x01; // END_STREAM
                header.stream_id = stream_id;

                return header.serialize() + data;
            }

            // Create and send a HEADERS frame
            std::string create_headers_frame(uint32_t stream_id, const std::string &encoded_headers)
            {
                Http2FrameHeader header;
                header.length = encoded_headers.length();
                header.type = Http2FrameType::HEADERS;
                header.flags = 0x05; // END_STREAM | END_HEADERS
                header.stream_id = stream_id;

                return header.serialize() + encoded_headers;
            }

            uint32_t get_next_stream_id()
            {
                uint32_t id = next_stream_id;
                next_stream_id += 2; // Client uses odd stream IDs
                return id;
            }

            int get_socket() const { return sockfd; }
            bool get_is_https() const { return is_https; }

        private:
            int sockfd;
            bool is_https;
            uint32_t next_stream_id;
            int32_t remote_flow_window;
            int32_t local_flow_window;
        };

    } // namespace detail

    // HTTP/2 Response wrapper
    class Http2Response : public Response
    {
    public:
        Http2Response() : stream_id(0) {}

        void set_stream_id(uint32_t id) { stream_id = id; }
        uint32_t get_stream_id() const { return stream_id; }

    private:
        uint32_t stream_id;
    };

} // namespace http

#endif

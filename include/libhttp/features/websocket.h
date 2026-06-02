#ifndef LIBHTTP_FEATURES_WEBSOCKET_H
#define LIBHTTP_FEATURES_WEBSOCKET_H

#include <string>
#include <functional>
#include <thread>
#include <mutex>
#include <memory>
#include <cstdint>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

namespace http
{
    namespace websocket
    {
        enum class OpCode : uint8_t
        {
            CONTINUATION = 0x0,
            TEXT = 0x1,
            BINARY = 0x2,
            CLOSE = 0x8,
            PING = 0x9,
            PONG = 0xA
        };

        // WebSocket Frame representation
        struct Frame
        {
            bool fin;
            OpCode opcode;
            bool masked;
            uint8_t mask_key[4];
            std::string payload;

            std::string serialize() const
            {
                std::string frame;

                // First byte: FIN + RSV + Opcode
                uint8_t byte1 = static_cast<uint8_t>(opcode);
                if (fin)
                    byte1 |= 0x80;
                frame.push_back(static_cast<char>(byte1));

                // Second byte: MASK + Payload length
                uint8_t byte2 = 0;
                if (masked)
                    byte2 |= 0x80;

                size_t payload_len = payload.size();
                if (payload_len < 126)
                {
                    byte2 |= payload_len;
                    frame.push_back(static_cast<char>(byte2));
                }
                else if (payload_len < 65536)
                {
                    byte2 |= 126;
                    frame.push_back(static_cast<char>(byte2));
                    frame.push_back(static_cast<char>((payload_len >> 8) & 0xFF));
                    frame.push_back(static_cast<char>(payload_len & 0xFF));
                }
                else
                {
                    byte2 |= 127;
                    frame.push_back(static_cast<char>(byte2));
                    for (int i = 7; i >= 0; --i)
                    {
                        frame.push_back(static_cast<char>((payload_len >> (i * 8)) & 0xFF));
                    }
                }

                // Mask key if needed
                if (masked)
                {
                    frame.push_back(static_cast<char>(mask_key[0]));
                    frame.push_back(static_cast<char>(mask_key[1]));
                    frame.push_back(static_cast<char>(mask_key[2]));
                    frame.push_back(static_cast<char>(mask_key[3]));

                    // Apply mask to payload
                    std::string masked_payload = payload;
                    for (size_t i = 0; i < masked_payload.size(); ++i)
                    {
                        masked_payload[i] ^= mask_key[i % 4];
                    }
                    frame += masked_payload;
                }
                else
                {
                    frame += payload;
                }

                return frame;
            }
        };

        // WebSocket message callbacks
        using OnMessageCallback = std::function<void(const std::string &)>;
        using OnBinaryCallback = std::function<void(const std::string &)>;
        using OnCloseCallback = std::function<void(int, const std::string &)>;
        using OnErrorCallback = std::function<void(const std::string &)>;

        // Main WebSocket connection class
        class Connection
        {
        public:
            Connection(int socket_fd, bool is_https = false)
                : sockfd(socket_fd), is_https(is_https),
                  is_connected(true), is_client(true)
            {
            }

            ~Connection()
            {
                if (is_connected)
                {
                    close_connection();
                }
            }

            // Send text message
            void send_text(const std::string &message)
            {
                Frame frame;
                frame.fin = true;
                frame.opcode = OpCode::TEXT;
                frame.masked = is_client;
                frame.payload = message;

                // Generate random mask key for client frames
                if (is_client)
                {
                    frame.mask_key[0] = rand() & 0xFF;
                    frame.mask_key[1] = rand() & 0xFF;
                    frame.mask_key[2] = rand() & 0xFF;
                    frame.mask_key[3] = rand() & 0xFF;
                }

                send_frame(frame);
            }

            // Send binary message
            void send_binary(const std::string &data)
            {
                Frame frame;
                frame.fin = true;
                frame.opcode = OpCode::BINARY;
                frame.masked = is_client;
                frame.payload = data;

                if (is_client)
                {
                    frame.mask_key[0] = rand() & 0xFF;
                    frame.mask_key[1] = rand() & 0xFF;
                    frame.mask_key[2] = rand() & 0xFF;
                    frame.mask_key[3] = rand() & 0xFF;
                }

                send_frame(frame);
            }

            // Send ping
            void send_ping(const std::string &payload = "")
            {
                Frame frame;
                frame.fin = true;
                frame.opcode = OpCode::PING;
                frame.masked = is_client;
                frame.payload = payload;

                if (is_client)
                {
                    frame.mask_key[0] = rand() & 0xFF;
                    frame.mask_key[1] = rand() & 0xFF;
                    frame.mask_key[2] = rand() & 0xFF;
                    frame.mask_key[3] = rand() & 0xFF;
                }

                send_frame(frame);
            }

            // Close connection
            void close_connection(int code = 1000, const std::string &reason = "")
            {
                if (!is_connected)
                    return;

                Frame frame;
                frame.fin = true;
                frame.opcode = OpCode::CLOSE;
                frame.masked = is_client;

                if (code != 0)
                {
                    frame.payload.push_back(static_cast<char>((code >> 8) & 0xFF));
                    frame.payload.push_back(static_cast<char>(code & 0xFF));
                    frame.payload += reason;
                }

                if (is_client)
                {
                    frame.mask_key[0] = rand() & 0xFF;
                    frame.mask_key[1] = rand() & 0xFF;
                    frame.mask_key[2] = rand() & 0xFF;
                    frame.mask_key[3] = rand() & 0xFF;
                }

                send_frame(frame);
                is_connected = false;
                ::close(sockfd);
            }

            // Register callbacks
            void on_message(OnMessageCallback cb) { message_callback = cb; }
            void on_binary(OnBinaryCallback cb) { binary_callback = cb; }
            void on_close(OnCloseCallback cb) { close_callback = cb; }
            void on_error(OnErrorCallback cb) { error_callback = cb; }

            // Start receiving loop (non-blocking for simplicity)
            void start_receive_loop()
            {
                if (!receive_thread.get())
                {
                    receive_thread = std::make_unique<std::thread>([this]() {
                        this->receive_loop();
                    });
                }
            }

            bool is_open() const { return is_connected; }

        private:
            int sockfd;
            bool is_https;
            bool is_connected;
            bool is_client;
            std::unique_ptr<std::thread> receive_thread;
            std::mutex send_mutex;

            OnMessageCallback message_callback;
            OnBinaryCallback binary_callback;
            OnCloseCallback close_callback;
            OnErrorCallback error_callback;

            void send_frame(const Frame &frame)
            {
                std::lock_guard<std::mutex> lock(send_mutex);
                std::string serialized = frame.serialize();

                if (is_https)
                {
                    // In real implementation, send via SSL
                    // For now, just write to socket
                    ::send(sockfd, serialized.data(), serialized.size(), 0);
                }
                else
                {
                    ::send(sockfd, serialized.data(), serialized.size(), 0);
                }
            }

            void receive_loop()
            {
                char buffer[4096];
                while (is_connected)
                {
                    ssize_t received = ::recv(sockfd, buffer, sizeof(buffer), 0);
                    if (received <= 0)
                    {
                        is_connected = false;
                        if (close_callback)
                        {
                            close_callback(1006, "Connection closed");
                        }
                        break;
                    }

                    // Parse frame (simplified)
                    // In production, use proper frame parsing
                    if (received > 0 && message_callback)
                    {
                        std::string payload(buffer, received);
                        message_callback(payload);
                    }
                }
            }
        };

        // Helper function to generate WebSocket key
        inline std::string generate_ws_key()
        {
            static const char *base64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
            std::string key(16, '\0');
            for (int i = 0; i < 16; ++i)
            {
                key[i] = static_cast<char>(rand() % 256);
            }

            // Simple base64 encode (for WebSocket key)
            std::string encoded;
            int val = 0;
            int valb = -6;
            for (unsigned char c : key)
            {
                val = (val << 8) + c;
                valb += 8;
                while (valb >= 0)
                {
                    encoded.push_back(base64_table[(val >> valb) & 0x3F]);
                    valb -= 6;
                }
            }
            if (valb > -6)
                encoded.push_back(base64_table[((val << 8) >> (valb + 8)) & 0x3F]);
            while (encoded.size() % 4)
                encoded.push_back('=');

            return encoded;
        }

        // Helper to generate Sec-WebSocket-Accept
        inline std::string generate_ws_accept(const std::string &key)
        {
            const std::string magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
            std::string combined = key + magic;

            unsigned char hash[SHA_DIGEST_LENGTH];
            SHA_CTX ctx;
            SHA1_Init(&ctx);
            SHA1_Update(&ctx, combined.data(), combined.size());
            SHA1_Final(hash, &ctx);

            // Base64 encode
            static const char *base64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
            std::string encoded;
            int val = 0;
            int valb = -6;
            for (unsigned char c : std::string(reinterpret_cast<const char *>(hash), SHA_DIGEST_LENGTH))
            {
                val = (val << 8) + c;
                valb += 8;
                while (valb >= 0)
                {
                    encoded.push_back(base64_table[(val >> valb) & 0x3F]);
                    valb -= 6;
                }
            }
            if (valb > -6)
                encoded.push_back(base64_table[((val << 8) >> (valb + 8)) & 0x3F]);
            while (encoded.size() % 4)
                encoded.push_back('=');

            return encoded;
        }

    } // namespace websocket

} // namespace http

#endif

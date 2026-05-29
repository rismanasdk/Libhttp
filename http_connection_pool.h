#ifndef LIBHTTP_CONNECTION_POOL_H
#define LIBHTTP_CONNECTION_POOL_H

#include <string>
#include <map>
#include <deque>
#include <memory>
#include <mutex>
#include <chrono>

namespace http
{
    namespace detail
    {
        // Represents a pooled connection
        struct PooledConnection
        {
            int socket_fd;
            std::string host;
            std::string port;
            bool is_https;
            std::chrono::system_clock::time_point last_used;
            bool in_use;

            PooledConnection(int fd, const std::string &h, const std::string &p, bool https)
                : socket_fd(fd), host(h), port(p), is_https(https),
                  last_used(std::chrono::system_clock::now()), in_use(false)
            {
            }

            std::string get_connection_key() const
            {
                return (is_https ? "https://" : "http://") + host + ":" + port;
            }

            bool is_expired(int timeout_seconds = 300) const
            {
                auto now = std::chrono::system_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - last_used);
                return duration.count() > timeout_seconds;
            }
        };

        // Connection Pool for keeping alive and reusing connections
        class ConnectionPool
        {
        public:
            explicit ConnectionPool(size_t max_size = 10, int idle_timeout = 300)
                : max_pool_size(max_size), idle_timeout_seconds(idle_timeout)
            {
            }

            ~ConnectionPool()
            {
                clear();
            }

            // Get or create a connection
            int acquire_connection(const std::string &host, const std::string &port, bool is_https)
            {
                std::lock_guard<std::mutex> lock(pool_mutex);

                std::string key = (is_https ? "https://" : "http://") + host + ":" + port;
                auto &connections = pools[key];

                // Try to find an available connection
                for (auto it = connections.begin(); it != connections.end(); ++it)
                {
                    if (!it->in_use && !it->is_expired(idle_timeout_seconds))
                    {
                        it->in_use = true;
                        it->last_used = std::chrono::system_clock::now();
                        return it->socket_fd;
                    }
                }

                // No available connection, should create new one
                return -1;
            }

            // Release a connection back to pool
            void release_connection(const std::string &host, const std::string &port,
                                  bool is_https, int socket_fd, bool reusable = true)
            {
                std::lock_guard<std::mutex> lock(pool_mutex);

                std::string key = (is_https ? "https://" : "http://") + host + ":" + port;
                auto &connections = pools[key];

                // Find and mark as not in use
                for (auto &conn : connections)
                {
                    if (conn.socket_fd == socket_fd)
                    {
                        if (reusable)
                        {
                            conn.in_use = false;
                            conn.last_used = std::chrono::system_clock::now();
                        }
                        else
                        {
                            close(socket_fd);
                            conn.socket_fd = -1;
                        }
                        return;
                    }
                }

                // Connection not in pool, close it if not reusable
                if (!reusable)
                {
                    close(socket_fd);
                }
            }

            // Store a new connection in pool
            bool store_connection(const std::string &host, const std::string &port,
                                bool is_https, int socket_fd)
            {
                std::lock_guard<std::mutex> lock(pool_mutex);

                std::string key = (is_https ? "https://" : "http://") + host + ":" + port;
                auto &connections = pools[key];

                // Check pool size limit
                if (connections.size() >= max_pool_size)
                {
                    // Evict oldest idle connection
                    auto oldest = connections.begin();
                    for (auto it = connections.begin(); it != connections.end(); ++it)
                    {
                        if (!it->in_use && it->last_used < oldest->last_used)
                        {
                            oldest = it;
                        }
                    }
                    close(oldest->socket_fd);
                    connections.erase(oldest);
                }

                connections.emplace_back(socket_fd, host, port, is_https);
                return true;
            }

            // Get pool statistics
            struct PoolStats
            {
                size_t total_connections;
                size_t active_connections;
                size_t idle_connections;
                size_t pools_count;
            };

            PoolStats get_stats() const
            {
                std::lock_guard<std::mutex> lock(pool_mutex);
                PoolStats stats = {0, 0, 0, pools.size()};

                for (const auto &p : pools)
                {
                    for (const auto &conn : p.second)
                    {
                        stats.total_connections++;
                        if (conn.in_use)
                        {
                            stats.active_connections++;
                        }
                        else
                        {
                            stats.idle_connections++;
                        }
                    }
                }

                return stats;
            }

            // Clear all connections
            void clear()
            {
                std::lock_guard<std::mutex> lock(pool_mutex);

                for (auto &p : pools)
                {
                    for (auto &conn : p.second)
                    {
                        if (conn.socket_fd != -1)
                        {
                            close(conn.socket_fd);
                        }
                    }
                }

                pools.clear();
            }

            // Cleanup expired connections
            void cleanup_expired()
            {
                std::lock_guard<std::mutex> lock(pool_mutex);

                for (auto &p : pools)
                {
                    auto &connections = p.second;
                    for (auto it = connections.begin(); it != connections.end();)
                    {
                        if (!it->in_use && it->is_expired(idle_timeout_seconds))
                        {
                            close(it->socket_fd);
                            it = connections.erase(it);
                        }
                        else
                        {
                            ++it;
                        }
                    }
                }
            }

            void set_max_pool_size(size_t size)
            {
                std::lock_guard<std::mutex> lock(pool_mutex);
                max_pool_size = size;
            }

            size_t get_max_pool_size() const
            {
                std::lock_guard<std::mutex> lock(pool_mutex);
                return max_pool_size;
            }

        private:
            mutable std::mutex pool_mutex;
            size_t max_pool_size;
            int idle_timeout_seconds;
            // Map of host:port -> list of connections
            std::map<std::string, std::deque<PooledConnection>> pools;
        };

    } // namespace detail

    // Global connection pool instance
    inline detail::ConnectionPool &global_connection_pool()
    {
        static detail::ConnectionPool pool(10, 300);
        return pool;
    }

    // Configuration for connection pooling
    struct PoolingConfig
    {
        bool enable_pooling = true;
        size_t max_pool_size = 10;
        int idle_timeout_seconds = 300;
        bool keep_alive = true;

        PoolingConfig() = default;

        explicit PoolingConfig(bool enable, size_t max_size = 10)
            : enable_pooling(enable), max_pool_size(max_size) {}
    };

} // namespace http

#endif

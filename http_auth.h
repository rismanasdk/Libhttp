#ifndef LIBHTTP_AUTH_H
#define LIBHTTP_AUTH_H

#include <string>
#include <map>

namespace http
{
    namespace auth
    {

        inline std::string basic(const std::string &username, const std::string &password)
        {
            extern std::string base64_encode(const std::string &value);
            return "Basic " + base64_encode(username + ":" + password);
        }

        inline std::string bearer(const std::string &token)
        {
            return "Bearer " + token;
        }

        inline std::string custom(const std::string &scheme, const std::string &credentials)
        {
            return scheme + " " + credentials;
        }

        struct DigestAuthContext
        {
            std::string realm;
            std::string nonce;
            std::string qop;
            std::string opaque;
            std::string algorithm;
            int nc = 1; 
            std::string cnonce;

            DigestAuthContext() = default;

            explicit DigestAuthContext(const std::string &www_authenticate_header)
            {
                parse_www_authenticate(www_authenticate_header);
            }

            std::string generate_response(const std::string &username, const std::string &password,
                                          const std::string &method, const std::string &uri) const;

        private:
            void parse_www_authenticate(const std::string &header)
            {
                if (header.substr(0, 7) != "Digest ")
                {
                    return;
                }

                std::string params = header.substr(7);
                std::string::size_type pos = 0;

                while (pos < params.length())
                {
                    std::string::size_type eq_pos = params.find('=', pos);
                    if (eq_pos == std::string::npos)
                        break;

                    std::string key = params.substr(pos, eq_pos - pos);
                    key.erase(key.find_last_not_of(" \t") + 1);
                    key.erase(0, key.find_first_not_of(" \t"));

                    std::string::size_type val_start = eq_pos + 1;
                    while (val_start < params.length() && (params[val_start] == ' ' || params[val_start] == '\t'))
                    {
                        val_start++;
                    }

                    std::string value;
                    if (val_start < params.length() && params[val_start] == '"')
                    {
                        val_start++;
                        std::string::size_type val_end = params.find('"', val_start);
                        if (val_end == std::string::npos)
                            break;
                        value = params.substr(val_start, val_end - val_start);
                        pos = val_end + 1;
                    }
                    else
                    {
                        std::string::size_type val_end = params.find(',', val_start);
                        if (val_end == std::string::npos)
                            val_end = params.length();
                        value = params.substr(val_start, val_end - val_start);
                        value.erase(value.find_last_not_of(" \t") + 1);
                        pos = val_end;
                    }

                    if (key == "realm")
                    {
                        realm = value;
                    }
                    else if (key == "nonce")
                    {
                        nonce = value;
                    }
                    else if (key == "qop")
                    {
                        qop = value;
                    }
                    else if (key == "opaque")
                    {
                        opaque = value;
                    }
                    else if (key == "algorithm")
                    {
                        algorithm = value;
                    }

                    while (pos < params.length() && (params[pos] == ',' || params[pos] == ' ' || params[pos] == '\t'))
                    {
                        pos++;
                    }
                }
            }
        };

        inline std::string api_key(const std::string &key)
        {
            return key;
        }

        inline std::string oauth2(const std::string &access_token)
        {
            return "Bearer " + access_token;
        }

    } // namespace auth
} // namespace http

#endif

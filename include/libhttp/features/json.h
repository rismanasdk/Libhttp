#ifndef LIBHTTP_FEATURES_JSON_H
#define LIBHTTP_FEATURES_JSON_H

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <cctype>
#include <cmath>

namespace http
{
    namespace json
    {

        class Value
        {
        public:
            enum class Type
            {
                Null,
                Boolean,
                Number,
                String,
                Array,
                Object
            };

            Type type;

            bool boolean_value = false;
            double number_value = 0.0;
            std::string string_value;
            std::vector<Value> array_value;
            std::map<std::string, Value> object_value;

            Value() : type(Type::Null) {}
            explicit Value(bool b) : type(Type::Boolean), boolean_value(b) {}
            explicit Value(double n) : type(Type::Number), number_value(n) {}
            explicit Value(int n) : type(Type::Number), number_value(static_cast<double>(n)) {}
            explicit Value(const std::string &s) : type(Type::String), string_value(s) {}
            explicit Value(const char *s) : type(Type::String), string_value(s) {}

            static Value null() { return Value(); }
            static Value boolean(bool b) { return Value(b); }
            static Value number(double n) { return Value(n); }
            static Value string(const std::string &s) { return Value(s); }
            static Value array()
            {
                Value v;
                v.type = Type::Array;
                return v;
            }
            static Value object()
            {
                Value v;
                v.type = Type::Object;
                return v;
            }

            bool is_null() const { return type == Type::Null; }
            bool is_boolean() const { return type == Type::Boolean; }
            bool is_number() const { return type == Type::Number; }
            bool is_string() const { return type == Type::String; }
            bool is_array() const { return type == Type::Array; }
            bool is_object() const { return type == Type::Object; }

            std::string dump(int indent = -1) const;
            static Value parse(const std::string &json_string);
        };

        inline std::string escape_string(const std::string &s)
        {
            std::string result;
            for (char c : s)
            {
                switch (c)
                {
                case '"':
                    result += "\\\"";
                    break;
                case '\\':
                    result += "\\\\";
                    break;
                case '\b':
                    result += "\\b";
                    break;
                case '\f':
                    result += "\\f";
                    break;
                case '\n':
                    result += "\\n";
                    break;
                case '\r':
                    result += "\\r";
                    break;
                case '\t':
                    result += "\\t";
                    break;
                default:
                    if (static_cast<unsigned char>(c) < 0x20)
                    {
                        char buf[8];
                        snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned char>(c));
                        result += buf;
                    }
                    else
                    {
                        result += c;
                    }
                }
            }
            return result;
        }

        inline std::string Value::dump(int indent) const
        {
            std::ostringstream oss;

            switch (type)
            {
            case Type::Null:
                oss << "null";
                break;
            case Type::Boolean:
                oss << (boolean_value ? "true" : "false");
                break;
            case Type::Number:
                if (std::floor(number_value) == number_value && std::abs(number_value) < 1e15)
                {
                    oss << static_cast<long long>(number_value);
                }
                else
                {
                    oss << number_value;
                }
                break;
            case Type::String:
                oss << "\"" << escape_string(string_value) << "\"";
                break;
            case Type::Array:
            {
                oss << "[";
                if (indent >= 0)
                    oss << "\n";
                for (size_t i = 0; i < array_value.size(); ++i)
                {
                    if (indent >= 0)
                    {
                        for (int j = 0; j < indent + 2; ++j)
                            oss << " ";
                    }
                    oss << array_value[i].dump(indent >= 0 ? indent + 2 : -1);
                    if (i < array_value.size() - 1)
                        oss << ",";
                    if (indent >= 0)
                        oss << "\n";
                }
                if (indent >= 0)
                {
                    for (int j = 0; j < indent; ++j)
                        oss << " ";
                }
                oss << "]";
                break;
            }
            case Type::Object:
            {
                oss << "{";
                if (indent >= 0)
                    oss << "\n";
                size_t i = 0;
                for (const auto &pair : object_value)
                {
                    if (indent >= 0)
                    {
                        for (int j = 0; j < indent + 2; ++j)
                            oss << " ";
                    }
                    oss << "\"" << escape_string(pair.first) << "\":";
                    if (indent >= 0)
                        oss << " ";
                    oss << pair.second.dump(indent >= 0 ? indent + 2 : -1);
                    if (i < object_value.size() - 1)
                        oss << ",";
                    if (indent >= 0)
                        oss << "\n";
                    ++i;
                }
                if (indent >= 0)
                {
                    for (int j = 0; j < indent; ++j)
                        oss << " ";
                }
                oss << "}";
                break;
            }
            }

            return oss.str();
        }

        inline Value Value::parse(const std::string &json_string)
        {
            size_t pos = 0;

            auto skip_whitespace = [&]()
            {
                while (pos < json_string.length() && std::isspace(json_string[pos]))
                {
                    ++pos;
                }
            };

            std::function<Value()> parse_value;
            parse_value = [&]() -> Value
            {
                skip_whitespace();
                if (pos >= json_string.length())
                    return Value::null();

                char c = json_string[pos];

                if (c == '"')
                {
                    ++pos;
                    std::string s;
                    while (pos < json_string.length() && json_string[pos] != '"')
                    {
                        if (json_string[pos] == '\\' && pos + 1 < json_string.length())
                        {
                            ++pos;
                            switch (json_string[pos])
                            {
                            case '"':
                            case '\\':
                            case '/':
                                s += json_string[pos];
                                break;
                            case 'b':
                                s += '\b';
                                break;
                            case 'f':
                                s += '\f';
                                break;
                            case 'n':
                                s += '\n';
                                break;
                            case 'r':
                                s += '\r';
                                break;
                            case 't':
                                s += '\t';
                                break;
                            default:
                                s += json_string[pos];
                            }
                        }
                        else
                        {
                            s += json_string[pos];
                        }
                        ++pos;
                    }
                    if (pos < json_string.length())
                        ++pos;
                    return Value::string(s);
                }
                else if (c == '[')
                {
                    Value arr = Value::array();
                    ++pos;
                    skip_whitespace();
                    if (pos < json_string.length() && json_string[pos] != ']')
                    {
                        while (pos < json_string.length())
                        {
                            arr.array_value.push_back(parse_value());
                            skip_whitespace();
                            if (pos < json_string.length() && json_string[pos] == ',')
                            {
                                ++pos;
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                    skip_whitespace();
                    if (pos < json_string.length())
                        ++pos;
                    return arr;
                }
                else if (c == '{')
                {
                    Value obj = Value::object();
                    ++pos;
                    skip_whitespace();
                    if (pos < json_string.length() && json_string[pos] != '}')
                    {
                        while (pos < json_string.length())
                        {
                            skip_whitespace();
                            Value key_val = parse_value();
                            if (!key_val.is_string())
                                break;
                            skip_whitespace();
                            if (pos < json_string.length() && json_string[pos] == ':')
                            {
                                ++pos;
                            }
                            Value val = parse_value();
                            obj.object_value[key_val.string_value] = val;
                            skip_whitespace();
                            if (pos < json_string.length() && json_string[pos] == ',')
                            {
                                ++pos;
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                    skip_whitespace();
                    if (pos < json_string.length())
                        ++pos;
                    return obj;
                }
                else if (c == 't' && json_string.substr(pos, 4) == "true")
                {
                    pos += 4;
                    return Value::boolean(true);
                }
                else if (c == 'f' && json_string.substr(pos, 5) == "false")
                {
                    pos += 5;
                    return Value::boolean(false);
                }
                else if (c == 'n' && json_string.substr(pos, 4) == "null")
                {
                    pos += 4;
                    return Value::null();
                }
                else if (c == '-' || std::isdigit(c))
                {
                    size_t start = pos;
                    if (c == '-')
                        ++pos;
                    while (pos < json_string.length() && std::isdigit(json_string[pos]))
                    {
                        ++pos;
                    }
                    if (pos < json_string.length() && json_string[pos] == '.')
                    {
                        ++pos;
                        while (pos < json_string.length() && std::isdigit(json_string[pos]))
                        {
                            ++pos;
                        }
                    }
                    if (pos < json_string.length() && (json_string[pos] == 'e' || json_string[pos] == 'E'))
                    {
                        ++pos;
                        if (pos < json_string.length() && (json_string[pos] == '+' || json_string[pos] == '-'))
                        {
                            ++pos;
                        }
                        while (pos < json_string.length() && std::isdigit(json_string[pos]))
                        {
                            ++pos;
                        }
                    }
                    return Value::number(std::stod(json_string.substr(start, pos - start)));
                }

                return Value::null();
            };

            return parse_value();
        }

    } // namespace json
} // namespace http

#endif

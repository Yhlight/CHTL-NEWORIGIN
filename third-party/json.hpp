// JSON for Modern C++ (nlohmann/json)
// Simplified placeholder for compilation
// In production, use the full library from: https://github.com/nlohmann/json

#ifndef NLOHMANN_JSON_HPP
#define NLOHMANN_JSON_HPP

#include <string>
#include <vector>
#include <map>
#include <sstream>

namespace nlohmann {

class json {
public:
    enum class value_t {
        null, object, array, string, boolean, number_integer, number_unsigned, number_float
    };
    
private:
    value_t type_ = value_t::null;
    std::map<std::string, json> object_;
    std::vector<json> array_;
    std::string string_;
    double number_ = 0.0;
    bool boolean_ = false;
    
public:
    json() = default;
    json(const std::string& s) : type_(value_t::string), string_(s) {}
    json(const char* s) : type_(value_t::string), string_(s) {}
    json(int n) : type_(value_t::number_integer), number_(n) {}
    json(double n) : type_(value_t::number_float), number_(n) {}
    json(bool b) : type_(value_t::boolean), boolean_(b) {}
    
    static json object() {
        json j;
        j.type_ = value_t::object;
        return j;
    }
    
    static json array() {
        json j;
        j.type_ = value_t::array;
        return j;
    }
    
    bool contains(const std::string& key) const {
        return object_.find(key) != object_.end();
    }
    
    json& operator[](const std::string& key) {
        if (type_ == value_t::null) type_ = value_t::object;
        return object_[key];
    }
    
    const json& operator[](const std::string& key) const {
        static json null_json;
        auto it = object_.find(key);
        return it != object_.end() ? it->second : null_json;
    }
    
    json& operator[](size_t idx) {
        if (type_ == value_t::null) type_ = value_t::array;
        if (idx >= array_.size()) array_.resize(idx + 1);
        return array_[idx];
    }
    
    void push_back(const json& j) {
        if (type_ == value_t::null) type_ = value_t::array;
        array_.push_back(j);
    }
    
    template<typename T>
    T get() const {
        if constexpr (std::is_same_v<T, std::string>) {
            return string_;
        } else if constexpr (std::is_same_v<T, bool>) {
            return boolean_;
        } else if constexpr (std::is_same_v<T, int>) {
            return static_cast<int>(number_);
        } else if constexpr (std::is_same_v<T, double>) {
            return number_;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::vector<std::string> result;
            for (const auto& item : array_) {
                result.push_back(item.string_);
            }
            return result;
        }
        return T();
    }
    
    operator std::string() const { return string_; }
    operator int() const { return static_cast<int>(number_); }
    operator double() const { return number_; }
    operator bool() const { return boolean_; }
    
    std::string dump(int indent = -1) const {
        std::ostringstream oss;
        dump_impl(oss, indent, 0);
        return oss.str();
    }
    
private:
    void dump_impl(std::ostringstream& oss, int indent, int depth) const {
        std::string ind = indent > 0 ? std::string(depth * indent, ' ') : "";
        std::string next_ind = indent > 0 ? std::string((depth + 1) * indent, ' ') : "";
        
        switch (type_) {
            case value_t::null:
                oss << "null";
                break;
            case value_t::boolean:
                oss << (boolean_ ? "true" : "false");
                break;
            case value_t::number_integer:
            case value_t::number_unsigned:
            case value_t::number_float:
                oss << number_;
                break;
            case value_t::string:
                oss << "\"" << string_ << "\"";
                break;
            case value_t::array:
                oss << "[";
                if (indent > 0) oss << "\n";
                for (size_t i = 0; i < array_.size(); ++i) {
                    if (indent > 0) oss << next_ind;
                    array_[i].dump_impl(oss, indent, depth + 1);
                    if (i < array_.size() - 1) oss << ",";
                    if (indent > 0) oss << "\n";
                }
                if (indent > 0) oss << ind;
                oss << "]";
                break;
            case value_t::object:
                oss << "{";
                if (indent > 0) oss << "\n";
                size_t count = 0;
                for (const auto& [key, value] : object_) {
                    if (indent > 0) oss << next_ind;
                    oss << "\"" << key << "\":";
                    if (indent > 0) oss << " ";
                    value.dump_impl(oss, indent, depth + 1);
                    if (++count < object_.size()) oss << ",";
                    if (indent > 0) oss << "\n";
                }
                if (indent > 0) oss << ind;
                oss << "}";
                break;
        }
    }
};

} // namespace nlohmann

#endif // NLOHMANN_JSON_HPP

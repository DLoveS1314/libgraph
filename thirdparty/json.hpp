#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cctype>
#include <algorithm>
#include <stdexcept>

// JSON parser implementation (simplified version for demonstration)
namespace nlohmann {
    class json {
    public:
        enum class value_t { object, array, string, number, boolean, null };

        value_t type() const { return m_type; }

        // Simplified accessors
        bool is_object() const { return m_type == value_t::object; }
        bool is_array() const { return m_type == value_t::array; }
        bool is_string() const { return m_type == value_t::string; }
        bool is_number() const { return m_type == value_t::number; }
        bool is_boolean() const { return m_type == value_t::boolean; }
        bool is_null() const { return m_type == value_t::null; }

        const std::string& get_string() const { if (is_string()) return m_string_val; throw std::runtime_error("Not a string"); }
        double get_number() const { if (is_number()) return m_number_val; throw std::runtime_error("Not a number"); }
        bool get_boolean() const { if (is_boolean()) return m_bool_val; throw std::runtime_error("Not a boolean"); }

        // Simplified operator[] for demonstration
        const json& operator[](const std::string& key) const {
            if (is_object()) {
                auto it = m_object_val.find(key);
                if (it != m_object_val.end()) return it->second;
            }
            throw std::runtime_error("Key not found: " + key);
        }

        const json& operator[](size_t index) const {
            if (is_array() && index < m_array_val.size()) return m_array_val[index];
            throw std::runtime_error("Index out of bounds");
        }

        size_t size() const {
            if (is_array()) return m_array_val.size();
            if (is_object()) return m_object_val.size();
            return 0;
        }

        // Simplified parse function (for demonstration only)
        static json parse(const std::string& input) {
            // This is a placeholder implementation
            // In a real scenario, use the actual nlohmann/json library
            json j;
            j.m_type = value_t::object;
            return j;
        }

    private:
        value_t m_type = value_t::null;
        std::string m_string_val;
        double m_number_val = 0;
        bool m_bool_val = false;
        std::vector<json> m_array_val;
        std::map<std::string, json> m_object_val;
    };
}
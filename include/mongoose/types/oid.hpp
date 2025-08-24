#ifndef QUARKPUNK_MONGOOSE_TYPE_OID_HPP
#define QUARKPUNK_MONGOOSE_TYPE_OID_HPP

#include<nlohmann/json.hpp>
#include<string>
#include<sstream>
#include<iomanip>
#include<cstring>

namespace mongoose { namespace type {

struct oid {
private:
    std::string value_;
public:
    oid() : value_() {}
    oid(const std::string& str) : value_(str) {}
    oid(const char* str) : value_(str) {}

    oid& operator=(std::string& str) {
        value_ = str;
        return *this;
    }

    oid& operator=(const std::string& str) {
        value_ = str;
        return *this;
    }
    
    oid& operator=(const char* str) {
        value_ = str;
        return *this;
    }

    oid& operator=(std::string&& str) {
        value_ = std::move(str);
        return *this;
    }
    
    bool operator==(const oid& other) const { return value_ == other.value_; }
    bool operator!=(const oid& other) const { return !(*this == other); }
    bool operator<(const oid& other) const { return value_ < other.value_; }
    bool operator>(const oid& other) const { return value_ > other.value_; }
    bool operator==(const std::string& str) const { return value_ == str; }
    bool operator!=(const std::string& str) const { return !(*this == str); }
    
    std::string to_string() const { return value_; }
    
    bool is_valid() const {
        if(value_.length() != 24) return false;
        for(char c : value_){
            if (!isxdigit(c)) return false;
        }
        return true;
    }

    static bool is_valid(const std::string& oid_string) {
        if (oid_string.length() != 24) return false;
        for (char c : oid_string) {
            if (!isxdigit(c)) return false;
        }
        return true;
    }

    explicit operator std::string() const { return value_; }
    bool empty() const { return value_.empty(); }
};

}}

namespace nlohmann {

template <>
struct adl_serializer<mongoose::type::oid> {
    static void to_json(json& j, const mongoose::type::oid& oid) {
        j = {{ "$oid", oid.to_string()}};
    }
    static void from_json(const json& j, mongoose::type::oid& oid) {
        // from raw string
        if(j.is_string()){
            oid = mongoose::type::oid{j.get<std::string>()};
            return;
        }
        // from mongodb
        if(!j.is_object() || j.is_null()) throw json::type_error::create(302, "Invalid OID format", &j);
        oid = mongoose::type::oid{j.at("$oid").get<std::string>()};
    }
};

}

#endif
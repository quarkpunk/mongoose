#ifndef QUARKPUNK_MONGOOSE_TYPE_DATE_HPP
#define QUARKPUNK_MONGOOSE_TYPE_DATE_HPP

#include<nlohmann/json.hpp>
#include<chrono>
#include<string>
#include<iomanip>
#include<sstream>

namespace mongoose { namespace type {

struct date {
private:
    std::chrono::system_clock::time_point time_;
public:
    date() : time_(std::chrono::system_clock::now()) {}
    explicit date(std::chrono::system_clock::time_point tp) : time_(tp) {}
    date(const char* str){ *this = from_string(str); }
    date(const std::string& iso_str) : time_(from_string(iso_str).time_) {}
    date(int64_t milliseconds) : time_(std::chrono::system_clock::time_point(std::chrono::milliseconds(milliseconds))) {}

    std::chrono::system_clock::time_point time() const { return time_; }

    int64_t to_milliseconds() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            time_.time_since_epoch()).count();
    }

    std::chrono::system_clock::time_point to_timepoint() const {
        return time_;
    }

    date& operator=(std::string& str) {
        *this = from_string(str);
        return *this;
    }

    date& operator=(const std::string& str) {
        *this = from_string(str);
        return *this;
    }
    
    date& operator=(const char* str) {
        *this = from_string(str);
        return *this;
    }

    bool operator==(const date& other) const { return time_ == other.time_; }
    bool operator!=(const date& other) const { return !(*this == other); }
    bool operator<(const date& other) const { return time_ < other.time_; }
    bool operator>(const date& other) const { return time_ > other.time_; }
    bool operator==(const std::chrono::system_clock::time_point& tp) const { return time_ == tp; }
    bool operator!=(const std::chrono::system_clock::time_point& tp) const { return !(*this == tp); }
    bool operator==(const std::string& iso_str) const { return to_string() == iso_str; }
    bool operator!=(const std::string& iso_str) const { return !(*this == iso_str); }
    
    // string (ISO 8601)
    std::string to_string() const {
        auto time = std::chrono::system_clock::to_time_t(time_);
        std::tm tm = *std::gmtime(&time);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
        return oss.str();
    }
    
    // string (ISO 8601)
    static date from_string(const std::string& iso_str) {
        std::tm tm = {};
        std::istringstream iss(iso_str);
        iss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
        if (iss.fail()) {
            throw std::runtime_error("Failed to parse date string: " + iso_str);
        }
        auto time = std::mktime(&tm);
        return date(std::chrono::system_clock::from_time_t(time));
    }

    date add_days(int days) const {
        return date(time_ + std::chrono::hours(24 * days));
    }
    
    date add_hours(int hours) const {
        return date(time_ + std::chrono::hours(hours));
    }
    
    date add_minutes(int minutes) const {
        return date(time_ + std::chrono::minutes(minutes));
    }
    
    date add_seconds(int seconds) const {
        return date(time_ + std::chrono::seconds(seconds));
    }
    
    date sub_days(int days) const {
        return date(time_ - std::chrono::hours(24 * days));
    }
    
    date sub_hours(int hours) const {
        return date(time_ - std::chrono::hours(hours));
    }
    
    date sub_minutes(int minutes) const {
        return date(time_ - std::chrono::minutes(minutes));
    }
    
    date sub_seconds(int seconds) const {
        return date(time_ - std::chrono::seconds(seconds));
    }
};

}}

namespace nlohmann {

template <>
struct adl_serializer<mongoose::type::date> {
    static void to_json(json& j, const mongoose::type::date& date){
        j = {{ "$date", date.to_string() }};
    }
    static void from_json(const json& j, mongoose::type::date& date){
        // from int (timestamp)
        if(j.is_number()){
            date = mongoose::type::date{j.get<int64_t>()};
            return;
        }
        // from string
        if(j.is_string()){
            date = mongoose::type::date::from_string(j.get<std::string>());
            return;
        }
        // from mongodb
        if(!j.is_object() || j.is_null()) throw json::type_error::create(302, "Invalid Date format", &j);
        date = mongoose::type::date{j.at("$date").get<int64_t>()};
    }
};

}

#endif
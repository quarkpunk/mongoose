#ifndef QUARKPUNK_MONGOOSE_TYPY_LOCATION_HPP
#define QUARKPUNK_MONGOOSE_TYPY_LOCATION_HPP

#include<mongoose/logger.hpp>
#include<nlohmann/json.hpp>
#include<string>
#include<sstream>
#include<iomanip>
#include<cstring>

namespace mongoose { namespace type {

struct location_point {
    double longitude;
    double latitude;

    location_point() : longitude(0.0), latitude(0.0) {}
    location_point(double log, double lat): longitude(log), latitude(lat) {}
    location_point(float log, float lat): longitude(log), latitude(lat) {}

    bool operator==(const location_point& other) {
        return other.longitude == longitude && other.latitude == latitude;
    }

    bool operator!=(const location_point& other) {
        return other.longitude != longitude && other.latitude != latitude;
    }
};

}}

namespace nlohmann {
template<>
struct adl_serializer<mongoose::type::location_point> {
    static void to_json(json& j, const mongoose::type::location_point& location) {
        j = json{
            {"type", "Point"},
            {"coordinates", json::array({
                location.longitude,
                location.latitude
            })}
        };
    }
    static void from_json(const json& j, mongoose::type::location_point& location) {
        if(j.at("type") != "Point") {
            mongoose::logger::log(mongoose::logger::WARN, "mongoose: GeoJSON type must be 'Point'");
            return;
        }
        const auto& coords = j.at("coordinates");
        if(!coords.is_array() || coords.size() != 2){
            mongoose::logger::log(mongoose::logger::WARN, "mongoose: coordinates must be array of 2 elements");
            return;
        }
        location.longitude = coords[0].get<double>();
        location.latitude = coords[1].get<double>();
    }
};
}

#endif
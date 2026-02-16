#ifndef QUARKPUNK_MONGOOSE_TYPE_GEO_POINT_HPP
#define QUARKPUNK_MONGOOSE_TYPE_GEO_POINT_HPP

#include<mongoose/logger.hpp>
#include<nlohmann/json.hpp>
#include<vector>
#include<string>

namespace mongoose::type::geo {

struct point {
private:
    std::pair<double, double> coordinates;
public:
    point() : coordinates{0, 0} {}
    explicit point(double longitude, double latitude) : coordinates{longitude, latitude} {}
    double get_longitude() const { return coordinates.first; }
    double get_latitude() const { return coordinates.second; }
    const auto& get_coordinates() const { return coordinates; }
    
    bool is_valid() const {
        return coordinates.first >= -180 && coordinates.first <= 180 &&
               coordinates.second >= -90 && coordinates.second <= 90;
    }
};

}

namespace nlohmann {

template <>
struct adl_serializer<mongoose::type::geo::point> {
    static void to_json(json& j, const mongoose::type::geo::point& p) {
        j = json{
            {"type", "Point"},
            {"coordinates", {p.get_longitude(), p.get_latitude()}}
        };
    }
    static void from_json(const json& j, mongoose::type::geo::point& p) {
        if(!j.is_object() || !j.contains("coordinates")){
            mongoose::logger::log(mongoose::logger::WARN, "mongoose: invalid GEO POINT type");
            return;
        }
        
        const auto& coords = j["coordinates"];
        if(coords.is_array() && coords.size() == 2) {
            p = mongoose::type::geo::point(
                coords[0].get<double>(),
                coords[1].get<double>()
            );
        }
    }
};

}

#endif
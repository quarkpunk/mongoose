#ifndef QUARKPUNK_MONGOOSE_TYPE_GEO_LINE_STRING_HPP
#define QUARKPUNK_MONGOOSE_TYPE_GEO_LINE_STRING_HPP

#include<mongoose/logger.hpp>
#include<nlohmann/json.hpp>
#include<vector>
#include<string>

namespace mongoose::type::geo {

struct line_string {
private:
    std::vector<std::pair<double, double>> coordinates;
public:
    line_string() = default;
    explicit line_string(std::vector<std::pair<double, double>> coords) 
        : coordinates(std::move(coords)) {}
    
    void add_point(double longitude, double latitude) {
        coordinates.emplace_back(longitude, latitude);
    }
    
    const auto& get_coordinates() const { return coordinates; }
    size_t size() const { return coordinates.size(); }
    
    bool is_valid() const {
        if (coordinates.size() < 2) return false;
        for (const auto& [lon, lat] : coordinates) {
            if (lon < -180 || lon > 180 || lat < -90 || lat > 90) return false;
        }
        return true;
    }
};

}

namespace nlohmann {

template <>
struct adl_serializer<mongoose::type::geo::line_string> {
    static void to_json(json& j, const mongoose::type::geo::line_string& ls) {
        json coords = json::array();
        for(const auto& [lon, lat] : ls.get_coordinates()) {
            coords.push_back({lon, lat});
        }
        j = json{
            {"type", "LineString"},
            {"coordinates", coords}
        };
    }
    static void from_json(const json& j, mongoose::type::geo::line_string& ls) {
        if(!j.is_object() || !j.contains("coordinates")){
            mongoose::logger::log(mongoose::logger::WARN, "mongoose: invalid GEO LINE STRING type");
            return;
        }
        const auto& coords = j["coordinates"];
        if(coords.is_array()) {
            for(const auto& point : coords) {
                if(point.is_array() && point.size() == 2) {
                    ls.add_point(
                        point[0].get<double>(),
                        point[1].get<double>()
                    );
                }
            }
        }
    }
};

}

#endif
#ifndef QUARKPUNK_MONGOOSE_TYPE_GEO_POLYGON_HPP
#define QUARKPUNK_MONGOOSE_TYPE_GEO_POLYGON_HPP

#include<mongoose/logger.hpp>
#include<nlohmann/json.hpp>
#include<vector>
#include<string>

namespace mongoose::type::geo {

class polygon {
private:
    // first array is the outer contour
    // the rest are openings (holes)
    std::vector<std::vector<std::pair<double, double>>> coordinates;  
public:
    polygon() = default;
    
    // add a contour (the first one is the outer one, the rest are holes)
    void add_ring(std::vector<std::pair<double, double>> ring) {
        coordinates.push_back(std::move(ring));
    }
    
    const auto& get_coordinates() const { return coordinates; }
    
    bool is_valid() const {
        if(coordinates.empty()) return false;
        // check the outer contour (at least 4 points, closed)
        const auto& exterior = coordinates[0];
        if(exterior.size() < 4) return false;
        // check for closure (first point == last)
        if(exterior.front() != exterior.back()) return false;
        // coordinate Validation
        for(const auto& ring : coordinates) {
            for (const auto& [lon, lat] : ring) {
                if (lon < -180 || lon > 180 || lat < -90 || lat > 90) return false;
            }
        }
        return true;
    }
};

}

namespace nlohmann{

template <>
struct adl_serializer<mongoose::type::geo::polygon> {
    static void to_json(json& j, const mongoose::type::geo::polygon& poly) {
        json coords = json::array();
        
        for(const auto& ring : poly.get_coordinates()) {
            json ring_json = json::array();
            for (const auto& [lon, lat] : ring) {
                ring_json.push_back({lon, lat});
            }
            coords.push_back(ring_json);
        }
        
        j = json{
            {"type", "Polygon"},
            {"coordinates", coords}
        };
    }
    
    static void from_json(const json& j, mongoose::type::geo::polygon& poly) {
        if(!j.is_object() || !j.contains("coordinates")){
            mongoose::logger::log(mongoose::logger::WARN, "mongoose: invalid GEO POLYGON type");
            return;
        }
        
        const auto& coords = j["coordinates"];
        if(coords.is_array()) {
            for(const auto& ring : coords) {
                std::vector<std::pair<double, double>> ring_points;
                if(ring.is_array()) {
                    for(const auto& point : ring) {
                        if(point.is_array() && point.size() == 2) {
                            ring_points.emplace_back(
                                point[0].get<double>(),
                                point[1].get<double>()
                            );
                        }
                    }
                }
                poly.add_ring(std::move(ring_points));
            }
        }
    }
};

}

#endif
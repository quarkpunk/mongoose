#ifndef MONGOOSE_TYPES_OID_HPP
#define MONGOOSE_TYPES_OID_HPP

#include <string>
#include <stdexcept>
#include <bsoncxx/oid.hpp>

namespace mongoose {
    using object_id = bsoncxx::oid;
    using oid = bsoncxx::oid;
}

namespace mongoose::types::oid {
    // from string
    inline object_id from_string(const std::string& str) {
        if(str.size() != 24) {
            throw std::length_error("json invalid object_id string: " + str);
        }
        return bsoncxx::oid{str};
    }

    // to string
    inline std::string to_string(const object_id& oid) {
        return oid.to_string();
    }

    // check equal
    inline bool equals(const object_id& a, const object_id& b) {
        return a == b;
    }

    // check is valid OID
    inline bool is_valid(const std::string& str) {
        try {
            from_string(str);
            return true;
        }
        catch(const std::exception& e) {}
        return false;
    }
}

// NLOHMANN_JSON type adapter
#ifdef MONGOOSE_USE_NLOHMANN_JSON
#include <nlohmann/json.hpp>

namespace nlohmann {

    template <>
    struct adl_serializer<mongoose::object_id> {
        static void to_json(json& j, const mongoose::object_id& value) {
            j = value.to_string();
        }
        static void from_json(const json& j, mongoose::object_id& value) {
            try {
                if (j.is_string()) {
                    value = mongoose::object_id{j.get<std::string>()};
                    return;
                }
            }
            catch (const std::exception&) {
                throw std::runtime_error("json failed parse object id");
            }
        }
    };

}

#endif // MONGOOSE_USE_NLOHMANN_JSON
#endif // MONGOOSE_TYPES_OID_HPP
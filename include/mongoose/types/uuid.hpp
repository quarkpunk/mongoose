#ifndef MONGOOSE_TYPES_UUID_HPP
#define MONGOOSE_TYPES_UUID_HPP

#include <string>
#include <stdexcept>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/string_generator.hpp>

namespace mongoose {
    using uuid = boost::uuids::uuid;
}

namespace mongoose::types::uuid {
    // from string
    inline mongoose::uuid from_string(const std::string& str) {
        try {
            boost::uuids::string_generator gen;
            boost::uuids::uuid uuid_new = gen(str);

            if (uuid_new.version() == boost::uuids::uuid::version_unknown) {
                throw std::runtime_error("json bad uuid string format: " + str);
            }

            return uuid_new;
        } 
        catch (const std::exception& e) {
            throw std::runtime_error("json failed parse uuid string: " + str);
        }
    }

    // to string
    inline std::string to_string(const mongoose::uuid& value) {
        return boost::uuids::to_string(value);
    }
}

// NLOHMANN_JSON type adapter
#ifdef MONGOOSE_USE_NLOHMANN_JSON
#include <nlohmann/json.hpp>

namespace nlohmann {
    template <>
    struct adl_serializer<mongoose::uuid> {
        static void to_json(json& j, const mongoose::uuid& value) {
            j = mongoose::types::uuid::to_string(value);
        }
        static void from_json(const json& j, mongoose::uuid& value) {
            try {
                if (j.is_string()) {
                    value = mongoose::types::uuid::from_string(j.get<std::string>());
                    return;
                }
            }
            catch (const std::exception&) {
                throw std::runtime_error("json failed parse uuid");
            }
        }
    };

}

#endif // MONGOOSE_USE_NLOHMANN_JSON
#endif // MONGOOSE_TYPES_UUID_HPP
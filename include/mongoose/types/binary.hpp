#ifndef QUARKPUNK_MONGOOSE_TYPE_BINARY_HPP
#define QUARKPUNK_MONGOOSE_TYPE_BINARY_HPP

#include<mongoose/json.hpp>
#include<mongoose/logger.hpp>
#include<bsoncxx/types.hpp>

namespace mongoose::type {

struct binary {
    std::string data;
    bsoncxx::binary_sub_type subtype;
    
    binary(){}
    explicit binary(std::string& value,
        bsoncxx::binary_sub_type subtype = bsoncxx::binary_sub_type::k_binary)
    : data(value), subtype(subtype){}
    explicit binary(const std::string& value,
        bsoncxx::binary_sub_type subtype = bsoncxx::binary_sub_type::k_binary)
    : data(value), subtype(subtype){}
};

}

namespace nlohmann {
template <>
struct adl_serializer<mongoose::type::binary> {
    static void to_json(json& j, const mongoose::type::binary& binary) {
        j = json{
            {"$binary", binary.data},
            {"$type", bsoncxx::to_string(binary.subtype)}
        };
    }
    static void from_json(const json& j, mongoose::type::binary& binary) {
        if(j.is_object() && (j.contains("$binary") && j.contains("$type"))) {
            binary.data = j["$binary"].get<std::string>();
            binary.subtype = static_cast<bsoncxx::binary_sub_type>(
                std::stoul(j["$binary"].get<std::string>(),
                nullptr, 16)
            );
            return;
        }
        // no value
        mongoose::logger::log(mongoose::logger::WARN, "mongoose: invalid $binary type");
    }
};

}

#endif
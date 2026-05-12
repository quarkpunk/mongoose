#ifndef QUARKPUNK_MONGOOSE_JSON_HPP
#define QUARKPUNK_MONGOOSE_JSON_HPP

// NLOHMANN_JSON type adapter
// NLOHMANN_JSON should already be included above
#ifdef NLOHMANN_JSON_VERSION_MAJOR

#include <string>
#include <optional>
#include <vector>
#include <mongoose/logger.hpp>

namespace mongoose::json {

    inline std::optional<nlohmann::json> parse(const std::string& json_str) noexcept {
        if (json_str.empty()) return std::nullopt;
        try {
            return nlohmann::json::parse(json_str);
        } catch (const nlohmann::json::exception& e) {
            logger::log(logger::ERROR, "json parse_json failed: %s", e.what());
            return std::nullopt;
        }
    }

    template<typename T>
    std::optional<T> from_string(const std::string& json_str) noexcept {
        if (json_str.empty()) return std::nullopt;
        try {
            auto json = nlohmann::json::parse(json_str);
            return json.get<T>();
        } catch (const nlohmann::json::exception& e) {
            logger::log(logger::ERROR, "json parse failed: %s", e.what());
            return std::nullopt;
        }
    }

    template<typename T>
    std::string to_string(const T& obj, int indent = -1) {
        try {
            nlohmann::json j = obj;
            return indent >= 0 ? j.dump(indent) : j.dump();
        } catch (const nlohmann::json::exception& e) {
            logger::log(logger::ERROR, "json serialize failed: %s", e.what());
            return "{}";
        }
    }

}

#endif // NLOHMANN_JSON_VERSION_MAJOR
#endif // QUARKPUNK_MONGOOSE_JSON_HPP
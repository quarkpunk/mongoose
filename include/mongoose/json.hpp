#ifndef QUARKPUNK_MONGOOSE_JSON_HPP
#define QUARKPUNK_MONGOOSE_JSON_HPP

#include <string>
#include <optional>
#include <mongoose/logger.hpp>

// json implementation
// special nlohmann_json macro
#ifdef MONGOOSE_USE_NLOHMANN_JSON
#include <nlohmann/json.hpp>

namespace nlohmann {

    // nlohmann_json type optional<T> adapter
    template <typename T>
    struct adl_serializer<std::optional<T>> {
        static void to_json(json& j, const std::optional<T>& opt) {
            if (opt.has_value()) j = opt.value();
            else j = nullptr;
        }
        static void from_json(const json& j, std::optional<T>& opt) {
            if (j.is_null()) opt = std::nullopt;
            else opt = j.get<T>();
        }
    };

}

namespace mongoose::json {

    template<typename T>
    inline std::optional<T> from_string(const std::string& json_str) noexcept {
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
    inline std::string to_string(const T& obj, int indent = -1) {
        try {
            nlohmann::json j = obj;
            return indent >= 0 ? j.dump(indent) : j.dump();
        } catch (const nlohmann::json::exception& e) {
            logger::log(logger::ERROR, "json serialize failed: %s", e.what());
            return "{}";
        }
    }

}

#else // fallback

namespace mongoose::json {

    template<typename T>
    inline std::optional<T> from_string(const std::string& json_str) noexcept {
        logger::log(logger::WARN, "json not implemented! check your linking libraries");
        return std::nullopt;
    }

    template<typename T>
    inline std::string to_string(const T& obj, int indent = -1) {
        logger::log(logger::WARN, "json not implemented! check your linking libraries");
        return "{}";
    }

}

#endif // MONGOOSE_USE_NLOHMANN_JSON
#endif // QUARKPUNK_MONGOOSE_JSON_HPP
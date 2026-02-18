#ifndef QUARKPUNK_MONGOOSE_MODEL_HPP
#define QUARKPUNK_MONGOOSE_MODEL_HPP

#include<mongoose/json.hpp>
#include<mongoose/logger.hpp>
#include<bsoncxx/document/value.hpp>
#include<bsoncxx/document/view.hpp>

#ifndef MONGOOSE_MODEL
#define MONGOOSE_MODEL NLOHMANN_DEFINE_TYPE_INTRUSIVE
#endif

// clear json for normalized string view
const inline nlohmann::json _mongoose_model_clean(const nlohmann::json& j){
    if(j.is_object()) {
        // check special bson types
        if(j.size() == 1) {
            if (j.contains("$oid")) return j["$oid"].get<std::string>();
            else if (j.contains("$date")) return j["$date"];
        }
        if(j.size() == 2){
            if (j.contains("$type") && j.contains("$binary")){
                return nlohmann::json{
                    { "binary", j["$binary"].get<std::string>() },
                    { "type", j["$type"].get<std::string>() },
                };
            }
        }
        // if not special type
        // process all fields recursively
        nlohmann::json cleaned;
        for(auto& [key, value] : j.items()) {
            cleaned[key] = _mongoose_model_clean(value);
        }
        return cleaned;
    }
    else if(j.is_array()) {
        nlohmann::json cleaned_arr;
        for(auto& item : j) {
            cleaned_arr.push_back(_mongoose_model_clean(item));
        }
        return cleaned_arr;
    }
    return j;
}

namespace mongoose::model{

template<typename T>
bool inline from_string(T& model, const std::string& json_str){
    try { model = nlohmann::json::parse(json_str).get<T>(); return true; }
    catch(const nlohmann::detail::exception& e){
        mongoose::logger::log(mongoose::logger::ERROR, "from_string > %s", e.what());
        return false;
    }
}

const std::string inline to_string(const nlohmann::json& model){
    try { return _mongoose_model_clean(model).dump(); }
    catch(const nlohmann::detail::exception& e) {
        mongoose::logger::log(mongoose::logger::ERROR, "to_string > %s", e.what());
        return {};
    }
}

template<typename T>
inline bool from_bson(T& model, const bsoncxx::document::value& bson) {
    try {
        const std::uint8_t* data_ptr = bson.data();
        const std::vector<std::uint8_t> vec(data_ptr, data_ptr + bson.length());
        model = nlohmann::json::from_bson(vec).get<T>();
        return true;
    }
    catch(const nlohmann::detail::exception& e) {
        mongoose::logger::log(mongoose::logger::ERROR, "from_bson > %s", e.what());
        return false;
    }
}

template<typename T>
const inline bsoncxx::document::value to_bson(const T& model) {
    const std::vector<std::uint8_t> bson_data = nlohmann::json::to_bson(nlohmann::json(model));
    const bsoncxx::document::view view(bson_data.data(),bson_data.size());
    return bsoncxx::document::value(view);
}

template<typename T>
const inline bsoncxx::document::value to_bson_without_id(const T& model) {
    nlohmann::json json_new = nlohmann::json(model);
    json_new.erase("_id");
    const std::vector<std::uint8_t> bson_data = nlohmann::json::to_bson(json_new);
    const bsoncxx::document::view view(bson_data.data(),bson_data.size());
    return bsoncxx::document::value(view);
}

}

#endif
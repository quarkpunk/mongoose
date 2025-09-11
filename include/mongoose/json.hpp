#ifndef QUARKPUNK_MONGOOSE_JSON_HPP
#define QUARKPUNK_MONGOOSE_JSON_HPP

#include<bsoncxx/document/value.hpp>
#include<bsoncxx/json.hpp>
#include<nlohmann/json.hpp>

#ifndef MODEL_JSON
#define MODEL_JSON NLOHMANN_DEFINE_TYPE_INTRUSIVE
#endif

using json_value = nlohmann::json;

// clear json for frontend view
const inline json_value _mongoose_json_clean(const json_value& j);

namespace mongoose{
namespace json{

template<typename T>
bool inline from_string(T& model, const std::string& json_str){
    if(json_str.empty()) return false;
    try { model = json_value::parse(json_str); return true; }
    catch(const nlohmann::detail::exception& e) {
        printf("from_string error -> %s\n", e.what());
        return false;
    }
    return true;
}

const std::string inline to_string(const json_value& model){
    try { return _mongoose_json_clean(model).dump(); }
    catch(const nlohmann::detail::exception& e) {
        printf("to_string error -> %s\n", e.what());
        return {};
    }
}

template<typename T>
const inline bsoncxx::document::value to_bson(T& model){
    return bsoncxx::from_json(json_value(model).dump());
}

template<typename T>
const inline bsoncxx::document::value to_bson_without_oid(T& model){
    json_value model_json = json_value(model);
    model_json.erase("_id");
    return bsoncxx::from_json(model_json.dump());
}

template<typename T>
const inline bool from_bson(T& model, const bsoncxx::document::value& bson){
    try { model = json_value::parse(bsoncxx::to_json(bson)); return true; }
    catch(const nlohmann::detail::exception& e) {
        printf("from_bson value error -> %s\n", e.what());
        return false;
    }
    return true;
}

template<typename T>
const inline bool from_bson(T& model, const bsoncxx::document::view& bson){
    try { model = json_value::parse(bsoncxx::to_json(bson)); return true; }
    catch(const nlohmann::detail::exception& e) {
        printf("from_bson view error -> %s\n", e.what());
        return false;
    }
    return true;
}

json_value inline clear_bson(const json_value& model){
    try { return _mongoose_json_clean(model).dump(); }
    catch(const nlohmann::detail::exception& e) {
        printf("clear_bson error -> %s\n", e.what());
        return {};
    }
}

}}

const inline json_value _mongoose_json_clean(const json_value& j){
    if (j.is_object()) {
        // check special bson types
        if (j.size() == 1) {
            if (j.contains("$oid")) {
                return j["$oid"].get<std::string>();
            } else if (j.contains("$date")) {
                return j["$date"];
            }
        }
        // if not special type
        // process all fields recursively
        json_value cleaned;
        for (auto& [key, value] : j.items()) {
            cleaned[key] = _mongoose_json_clean(value);
        }
        return cleaned;
    } else if (j.is_array()) {
        json_value cleaned_arr;
        for (auto& item : j) {
            cleaned_arr.push_back(_mongoose_json_clean(item));
        }
        return cleaned_arr;
    }
    return j;
}

#endif
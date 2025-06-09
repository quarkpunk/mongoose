#ifndef QUARKPUNK_MONGOOSE_MODEL_HPP
#define QUARKPUNK_MONGOOSE_MODEL_HPP

#include<nlohmann/json.hpp>

using json_value = nlohmann::json;

#ifndef JSON_MODEL
#define JSON_MODEL NLOHMANN_DEFINE_TYPE_INTRUSIVE
#endif

namespace mongoose{
namespace traits{

template <typename T, typename = void>
struct has_string_id_field : std::false_type {};

template <typename T>
struct has_string_id_field<T, std::void_t<decltype(std::declval<T>().id)>> 
    : std::is_convertible<decltype(std::declval<T>().id), std::string> {};

template <typename T>
std::optional<std::string> get_id(const T& object){
    if constexpr (has_string_id_field<T>::value){
        return object.id;
    }
    return std::nullopt;
}

template <typename T>
void set_id(T& object, const std::string& id){
    if constexpr (has_string_id_field<T>::value){
        object.id = id;
    }
}

template <typename T>
void get_id_to_json(T& object, json_value& value){
    std::optional<std::string> result = traits::get_id(object);
    if(result) value["id"] = result.value();
}

template <typename T>
void set_id_from_json(T& object, json_value& value){
    if(value["_id"]["$oid"].is_string()){
        set_id(object, value["_id"]["$oid"]);
    }
}}

namespace model{

// return true if model is valid, and false if not valid
template<typename T>
bool inline from_string(T& model, const std::string& json_str){
    if(json_str.empty()) return false;
    try { model = json_value::parse(json_str); return true; }
    catch(const nlohmann::detail::exception& e) {
        printf("model: from_string<%s> error -> %s\n", typeid(T).name(), e.what());
        return false;
    }
    return true;
}
// dump json to string
template<typename T>
const std::string inline to_string(T& model){
    try { return json_value(model).dump(); }
    catch(const nlohmann::detail::exception& e) {
        printf("model: to_string<%s> error -> %s\n", typeid(T).name(), e.what());
        return {};
    }
}
// dump std::vector<T> to string json array
template<typename T>
const std::string inline to_string_array(std::vector<T>& models){
    try{
        if(models.empty()) throw "array is empty";
        json_value document_json = json_value::array();
        for(const auto& elem : models){
            json_value json = json_value(elem);
            traits::get_id_to_json(elem, json);
            document_json.push_back(json);
        }
        return document_json.dump();
    } catch(const nlohmann::detail::exception& e) {
        printf("model: to_string_array<%s> error: %s\n", typeid(T).name(), e.what());
        return {};
    }
}
// parse string json to <T> model
// auto paste mongo oid to "id" field in your model (if exist)
// return true if success
template<typename T>
bool inline from_string_with_id(T& model, const std::string& json_str){
    if(json_str.empty()) return false;
    try {
        nlohmann::json value = nlohmann::json::parse(json_str);
        model = value;
        if(value["_id"]["$oid"].is_string()) traits::set_id(model, value["_id"]["$oid"]);
        return true;
    } catch(const nlohmann::detail::exception& e) {
        printf("from_string_with_id<%s> error: %s\n", typeid(T).name(), e.what());
        return false;
    }
    return true;
}
// dump <T> model to string json (dump)
// auto paste "id" model field in json string (if exist)
template<typename T>
const std::string inline to_string_with_id(T& model){
    try{
        nlohmann::json document_json = model;
        traits::get_id_to_json(model, document_json);
        return document_json.dump();
    } catch(const nlohmann::detail::exception& e){
        printf("to_string_with_id<%s> error: %s\n", typeid(T).name(), e.what());
        return {};
    }
}
// convert mongodb json to user json model id
const void inline from_json_mongo_id(json_value& value){
    try{
        if(value["_id"].empty()) return;
        value["id"] = value["_id"]["$oid"];
        value.erase("_id");
    } catch(const nlohmann::detail::exception& e){
        printf("from_mongo_to_json_id error: %s\n", e.what());
    }
}

}}

#endif
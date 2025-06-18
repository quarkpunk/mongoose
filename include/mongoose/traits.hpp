#ifndef QUARKPUNK_MONGOOSE_TRAITS_HPP
#define QUARKPUNK_MONGOOSE_TRAITS_HPP

#include<nlohmann/json.hpp>
#include<optional>

using json_value = nlohmann::json;

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
}

}}

#endif
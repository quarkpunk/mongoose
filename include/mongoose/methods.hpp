#ifndef QUARKPUNK_MONGOOSE_METHODS_HPP
#define QUARKPUNK_MONGOOSE_METHODS_HPP

// these are the main commonly used methods of mongoose
// all these methods have been templated here
// for convenience and reuse in your application repositories

#include<mongoose/mongoose.hpp>
#include<mongoose/model.hpp>

namespace mongoose{

// main methods

template <typename T>
bool find_all(std::vector<T>& results, mongo_collection& coll, const json_value& filter, const options_find& option_find = options_find()){
    std::vector<json_value> array = mongoose::base::find_all(coll, filter, option_find);
    std::vector<T> values;
    for(auto&& elem : array){
        T value = elem;
        mongoose::traits::set_id_from_json(value, elem);
        values.push_back(value);
    }
    results = values;
    return !results.empty();
}

template <typename T>
bool find_all(std::vector<T>& results, mongo_collection& coll, const json_value& filter, int skip = 0, int max = 10){
    std::vector<json_value> array = mongoose::base::find_all(coll, filter, skip, max);
    std::vector<T> values;
    for(auto&& elem : array){
        T value = elem;
        mongoose::traits::set_id_from_json(value, elem);
        values.push_back(value);
    }
    results = values;
    return !results.empty();
}

template <typename T>
bool find_all(std::vector<T>& results, mongo_collection& coll, int skip = 0, int max = 10){
    std::vector<json_value> array = mongoose::base::find_all(coll, skip, max);
    std::vector<T> values;
    for(auto&& elem : array){
        T value = elem;
        mongoose::traits::set_id_from_json(value, elem);
        values.push_back(value);
    }
    results = values;
    return !results.empty();
}

template <typename T>
bool find_all(std::vector<T>& results, mongo_collection& coll, std::vector<std::string>& ids){
    if(ids.empty()) return false;
    std::vector<json_value> array = mongoose::base::find_all(coll, ids, 0, ids.size());
    std::vector<T> values;
    for(auto&& elem : array){
        T value = elem;
        mongoose::traits::set_id_from_json(value, elem);
        values.push_back(value);
    }
    results = values;
    return !results.empty();
}

template <typename T>
bool find_id(T& result, mongo_collection& coll, const std::string& id){
    auto value = mongoose::base::find_id(coll, id);
    if(!value) return false;
    result = value.value();
    mongoose::traits::set_id_from_json(result, value.value());
    return true;
}

template <typename T>
bool find_one(T& result, mongo_collection& coll, const json_value& filter){
    auto value = mongoose::base::find_one(coll, filter);
    if(!value) return false;
    result = value.value();
    mongoose::traits::set_id_from_json(result, value.value());
    return true;
}

template <typename T>
bool insert_id(const T& data, mongo_collection& coll, const std::string& id){
    return mongoose::base::insert_id(coll, id, data);
}

template <typename T>
bool insert(const T& data, mongo_collection& coll, std::string& out_id){
    out_id = mongoose::base::insert(coll, data);
    return !out_id.empty();
}

template <typename T>
bool insert_many(const std::vector<T>& inserts, mongo_collection& coll){
    if(inserts.empty()) return false;
    std::vector<json_value> values;
    for(const auto& value : inserts){
        values.push_back(value);
    }
    return mongoose::base::insert_many(coll, values);
}

template <typename T>
bool insert_many(const std::vector<T>& inserts, mongo_collection& coll, std::vector<std::string>& return_oids){
    if(inserts.empty()) return false;
    std::vector<json_value> values;
    for(const auto& value : inserts){
        values.push_back(value);
    }
    return mongoose::base::insert_many(coll, values, return_oids);
}

template <typename T>
bool insert_many(const std::vector<T>& inserts, mongo_session& session, mongo_collection& coll, std::vector<std::string>& return_oids){
    if(inserts.empty()) return false;
    std::vector<json_value> values;
    for(const auto& value : inserts){
        values.push_back(value);
    }
    return mongoose::base::insert_many(session, coll, values, return_oids);
}

template <typename T>
bool update_id(T& data, mongo_collection& coll, const std::string& id){
    return mongoose::base::update_id(coll, id, data);
}

bool remove_id(mongo_collection& coll, const std::string& id){
    return mongoose::base::remove_id(coll, id);
}

// session methods

template <typename T>
bool find_all(std::vector<T>& results, mongo_session& session, mongo_collection& coll, const json_value& filter, const options_find& option_find = options_find()){
    std::vector<json_value> array = mongoose::base::find_all(session, coll, filter, option_find);
    std::vector<T> values;
    for(auto&& elem : array){
        T value = elem;
        mongoose::traits::set_id_from_json(value, elem);
        values.push_back(value);
    }
    results = values;
    return !results.empty();
}

template <typename T>
bool find_all(std::vector<T>& results, mongo_session& session, mongo_collection& coll, const json_value& filter, int skip = 0, int max = 10){
    std::vector<json_value> array = mongoose::base::find_all(session, coll, filter, skip, max);
    std::vector<T> values;
    for(auto&& elem : array){
        T value = elem;
        mongoose::traits::set_id_from_json(value, elem);
        values.push_back(value);
    }
    results = values;
    return !results.empty();
}

template <typename T>
bool find_all(std::vector<T>& results, mongo_session& session, mongo_collection& coll, int skip = 0, int max = 10){
    std::vector<json_value> array = mongoose::base::find_all(session, coll, skip, max);
    std::vector<T> values;
    for(auto&& elem : array){
        T value = elem;
        mongoose::traits::set_id_from_json(value, elem);
        values.push_back(value);
    }
    results = values;
    return !results.empty();
}

template <typename T>
bool find_all(std::vector<T>& results, mongo_session& session, mongo_collection& coll, std::vector<std::string>& ids){
    if(ids.empty()) return false;
    std::vector<json_value> array = mongoose::base::find_all(session, coll, ids, 0, ids.size());
    std::vector<T> values;
    for(auto&& elem : array){
        T value = elem;
        mongoose::traits::set_id_from_json(value, elem);
        values.push_back(value);
    }
    results = values;
    return !results.empty();
}

template <typename T>
bool find_id(T& result, mongo_session& session, mongo_collection& coll, const std::string& id){
    auto value = mongoose::base::find_id(session, coll, id);
    if(!value) return false;
    result = value.value();
    mongoose::traits::set_id_from_json(result, value.value());
    return true;
}

template <typename T>
bool find_one(T& result, mongo_session& session, mongo_collection& coll, const json_value& filter){
    auto value = mongoose::base::find_one(session, coll, filter);
    if(!value) return false;
    result = value.value();
    mongoose::traits::set_id_from_json(result, value.value());
    return true;
}

template <typename T>
bool insert_id(const T& data, mongo_session& session, mongo_collection& coll, const std::string& id){
    return mongoose::base::insert_id(session, coll, id, data);
}

template <typename T>
bool insert(const T& data, mongo_session& session, mongo_collection& coll, std::string& out_id){
    out_id = mongoose::base::insert(session, coll, data);
    return !out_id.empty();
}

template <typename T>
bool update_id(T& data, mongo_session& session, mongo_collection& coll, const std::string& id){
    return mongoose::base::update_id(session, coll, id, data);
}

bool remove_id(mongo_session& session, mongo_collection& coll, const std::string& id){
    return mongoose::base::remove_id(session, coll, id);
}

// bulk methods

template <typename T>
void bulk_insert_append(bulk_write& bulk, const std::vector<T>& inserts){
    std::vector<json_value> values;
    for(const auto& value : inserts){
        values.push_back(value);
    }
    mongoose::base::bulk_insert_append(bulk, values);
}

template <typename T>
void bulk_update_append(bulk_write& bulk, const std::vector<T>& updates){
    std::vector<std::pair<std::string, json_value>> datas;
    for(const auto& value : updates){
        auto id = mongoose::traits::get_id(value);
        if(!id) continue;
        std::pair<std::string, json_value> value_pair = {id.value(), value};
        datas.push_back(value_pair);
    }
    mongoose::base::bulk_update_append(bulk, datas);
}

void bulk_remove_append(bulk_write& bulk, const std::vector<std::string>& removes_id){
    mongoose::base::bulk_remove_append(bulk, removes_id);
}

// json methods

template <typename T>
bool json_find_all(json_value& results, mongo_collection& coll, const json_value& filter, const options_find& option_find = options_find()){
    std::vector<json_value> array = mongoose::base::find_all(coll, filter, option_find);
    json_value values = json_value::array();
    for(auto&& elem : array){
        mongoose::model::from_json_mongo_id(elem);
        values.push_back(elem);
    }
    results = values;
    return !results.empty();
}

bool json_find_all(json_value& results, mongo_collection& coll, const json_value& filter, int skip = 0, int max = 10){
    std::vector<json_value> array = mongoose::base::find_all(coll, filter, skip, max);
    json_value values = json_value::array();
    for(auto&& elem : array){
        mongoose::model::from_json_mongo_id(elem);
        values.push_back(elem);
    }
    results = values;
    return !results.empty();
}

bool json_find_all(json_value& results, mongo_collection& coll, int skip = 0, int max = 10){
    std::vector<json_value> array = mongoose::base::find_all(coll, skip, max);
    json_value values = json_value::array();
    for(auto&& elem : array){
        mongoose::model::from_json_mongo_id(elem);
        values.push_back(elem);
    }
    results = values;
    return !results.empty();
}

bool json_find_id(json_value& result, mongo_collection& coll, const std::string& id){
    auto value = mongoose::base::find_id(coll, id);
    if(!value) return false;
    result = value.value();
    mongoose::model::from_json_mongo_id(result);
    return true;
}

bool json_find_one(json_value& result, mongo_collection& coll, const json_value& filter){
    auto value = mongoose::base::find_one(coll, filter);
    if(!value) return false;
    result = value.value();
    mongoose::model::from_json_mongo_id(result);
    return true;
}

bool json_insert_id(const json_value& data, mongo_collection& coll, const std::string& id){
    return mongoose::base::insert_id(coll, id, data);
}

bool json_insert(const json_value& data, mongo_collection& coll, std::string& out_id){
    out_id = mongoose::base::insert(coll, data);
    return !out_id.empty();
}

bool json_update_id(const json_value& data, mongo_collection& coll, const std::string& id){
    return mongoose::base::update_id(coll, id, data);
}

}

#endif
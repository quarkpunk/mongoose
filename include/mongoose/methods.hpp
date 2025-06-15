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
bool find_all(mongo_collection& coll, std::vector<T>& result, const json_value& filter, int skip = 0, int max = 10){
    std::vector<json_value> array = mongoose::base::find_all(coll, filter, skip, max);
    std::vector<T> values;
    for(auto&& elem : array){
        T value = elem;
        mongoose::traits::set_id_from_json(value, elem);
        values.push_back(value);
    }
    result = values;
    return !result.empty();
}

template <typename T>
bool find_all(mongo_collection& coll, std::vector<T>& result, int skip = 0, int max = 10){
    std::vector<json_value> array = mongoose::base::find_all(coll, skip, max);
    std::vector<T> values;
    for(auto&& elem : array){
        T value = elem;
        mongoose::traits::set_id_from_json(value, elem);
        values.push_back(value);
    }
    result = values;
    return !result.empty();
}

template <typename T>
bool find_id(mongo_collection& coll, const std::string& id, T& result){
    auto value = mongoose::base::find_by_id(coll, id);
    if(!value) return false;
    result = value.value();
    mongoose::traits::set_id_from_json(result, value.value());
    return true;
}

template <typename T>
bool find_one(mongo_collection& coll, T& result, const json_value& filter){
    auto value = mongoose::base::find_one(coll, filter);
    if(!value) return false;
    result = value.value();
    mongoose::traits::set_id_from_json(result, value.value());
    return true;
}

template <typename T>
bool insert_id(mongo_collection& coll, const T& data, const std::string& id){
    return mongoose::base::insert_id(coll, id, data);
}

template <typename T>
bool insert(mongo_collection& coll, const T& data, std::string& out_id){
    out_id = mongoose::base::insert(coll, data);
    return !out_id.empty();
}

template <typename T>
bool update_id(mongo_collection& coll, const std::string& id, T& data){
    return mongoose::base::update_id(coll, id, data);
}

bool remove_id(mongo_collection& coll, const std::string& id){
    return mongoose::base::remove_id(coll, id);
}

// session methods

template <typename T>
bool find_all(mongo_session& session, mongo_collection& coll, std::vector<T>& result, const json_value& filter, int skip = 0, int max = 10){
    std::vector<json_value> array = mongoose::session::find_all(session, coll, filter, skip, max);
    std::vector<T> values;
    for(auto&& elem : array){
        T value = elem;
        mongoose::traits::set_id_from_json(value, elem);
        values.push_back(value);
    }
    result = values;
    return !result.empty();
}

template <typename T>
bool find_all(mongo_session& session, mongo_collection& coll, std::vector<T>& result, int skip = 0, int max = 10){
    std::vector<json_value> array = mongoose::session::find_all(session, coll, skip, max);
    std::vector<T> values;
    for(auto&& elem : array){
        T value = elem;
        mongoose::traits::set_id_from_json(value, elem);
        values.push_back(value);
    }
    result = values;
    return !result.empty();
}

template <typename T>
bool find_id(mongo_session& session, mongo_collection& coll, const std::string& id, T& result){
    auto value = mongoose::session::find_by_id(session, coll, id);
    if(!value) return false;
    result = value.value();
    mongoose::traits::set_id_from_json(result, value.value());
    return true;
}

template <typename T>
bool find_one(mongo_session& session, mongo_collection& coll, T& result, const json_value& filter){
    auto value = mongoose::session::find_one(session, coll, filter);
    if(!value) return false;
    result = value.value();
    mongoose::traits::set_id_from_json(result, value.value());
    return true;
}

template <typename T>
bool insert_id(mongo_session& session, mongo_collection& coll, const T& data, const std::string& id){
    return mongoose::session::insert_id(session, coll, id, data);
}

template <typename T>
bool insert(mongo_session& session, mongo_collection& coll, const T& data, std::string& out_id){
    out_id = mongoose::session::insert(session, coll, data);
    return !out_id.empty();
}

template <typename T>
bool update_id(mongo_session& session, mongo_collection& coll, const std::string& id, T& data){
    return mongoose::session::update_id(session, coll, id, data);
}

bool remove_id(mongo_session& session, mongo_collection& coll, const std::string& id){
    return mongoose::session::remove_id(session, coll, id);
}

// json methods

bool json_find_all(mongo_collection& coll, json_value& result_data, const json_value& filter, int skip = 0, int max = 10){
    std::vector<json_value> array = mongoose::base::find_all(coll, filter, skip, max);
    json_value values;
    for(auto&& elem : array){
        mongoose::model::from_json_mongo_id(elem);
        values.push_back(elem);
    }
    result_data = values;
    return !result_data.empty();
}

bool json_find_all(mongo_collection& coll, json_value& result_data, int skip = 0, int max = 10){
    std::vector<json_value> array = mongoose::base::find_all(coll, skip, max);
    json_value values;
    for(auto&& elem : array){
        mongoose::model::from_json_mongo_id(elem);
        values.push_back(elem);
    }
    result_data = values;
    return !result_data.empty();
}

bool json_find_id(mongo_collection& coll, const std::string& id, json_value& result){
    auto value = mongoose::base::find_by_id(coll, id);
    if(!value) return false;
    result = value.value();
    mongoose::model::from_json_mongo_id(result);
    return true;
}

bool json_find_one(mongo_collection& coll, json_value& result, const json_value& filter){
    auto value = mongoose::base::find_one(coll, filter);
    if(!value) return false;
    result = value.value();
    mongoose::model::from_json_mongo_id(result);
    return true;
}

bool json_insert_id(mongo_collection& coll, const json_value& data, const std::string& id){
    return mongoose::base::insert_id(coll, id, data);
}

bool json_insert(mongo_collection& coll, const json_value& data, std::string& out_id){
    out_id = mongoose::base::insert(coll, data);
    return !out_id.empty();
}

bool json_update_id(mongo_collection& coll, const std::string& id, json_value& data){
    return mongoose::base::update_id(coll, id, data);
}

}

#endif
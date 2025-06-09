#ifndef QUARKPUNK_MONGOOSE_REPOSITORY_HPP
#define QUARKPUNK_MONGOOSE_REPOSITORY_HPP

#include<mongoose/mongodb.hpp>
#include<mongoose/mongoose.hpp>
#include<mongoose/model.hpp>

namespace mongoose{

template <typename T>
class repository{
public:
    repository(mongoose::mongodb* mongodb, const std::string database, const std::string collection){
        this->mongo = mongodb;
        this->database = database;
        this->collection = collection;
    }
    virtual ~repository() = default;
    repository(const repository&) = delete;
    repository& operator=(const repository&) = delete;
public:
    virtual bool find_all(std::vector<T>& out_datas, const json_value& filter, int skip = 0, int max = 10){
        auto conn = mongo->pool.acquire();
        auto coll = conn->database(database).collection(collection);
        std::vector<json_value> array = mongoose::find_all(coll, filter, skip, max);
        std::vector<T> values;
        for(auto&& elem : array){
            T value = json_value::parse(elem.dump());
            mongoose::traits::set_id_from_json(value, elem);
            values.push_back(value);
        }
        out_datas = values;
        return !out_datas.empty();
    };
    virtual bool find_all(std::vector<T>& out_datas, int skip = 0, int max = 10){
        auto conn = mongo->pool.acquire();
        auto coll = conn->database(database).collection(collection);
        std::vector<json_value> array = mongoose::find_all(coll, skip, max);
        std::vector<T> values;
        for(auto&& elem : array){
            T value = json_value::parse(elem.dump());
            mongoose::traits::set_id_from_json(value, elem);
            values.push_back(value);
        }
        out_datas = values;
        return !out_datas.empty();
    };
    virtual bool find_id(const std::string& id, T& out_data){
        auto conn = mongo->pool.acquire();
        auto coll = conn->database(database).collection(collection);
        auto value = mongoose::find_by_id(coll, id);
        if(!value.has_value()) return false;
        out_data = value.value();
        mongoose::traits::set_id_from_json(out_data, value.value());
        return true;
    };
    virtual bool create_id(const T& data, const std::string& id){
        auto conn = mongo->pool.acquire();
        auto coll = conn->database(database).collection(collection);
        return mongoose::insert_id(coll, id, data);
    };
    virtual bool create(const T& data, std::string& out_id){
        auto conn = mongo->pool.acquire();
        auto coll = conn->database(database).collection(collection);
        out_id = mongoose::insert(coll, data);
        return !out_id.empty();
    };
    virtual bool update(const std::string& id, T& data){
        auto conn = mongo->pool.acquire();
        auto coll = conn->database(database).collection(collection);
        return mongoose::update_id(coll, id, data);
    };
    virtual bool remove(const std::string& id){
        auto conn = mongo->pool.acquire();
        auto coll = conn->database(database).collection(collection);
        return mongoose::remove_id(coll, id);
    };
    virtual bool json_find_all(json_value& out_array, const json_value& filter, int skip = 0, int max = 10){
        auto conn = mongo->pool.acquire();
        auto coll = conn->database(database).collection(collection);
        std::vector<json_value> array = mongoose::find_all(coll, filter, skip, max);
        json_value values;
        for(auto&& elem : array){
            mongoose::model::from_json_mongo_id(elem);
            values.push_back(elem);
        }
        out_array = values;
        return !out_array.empty();
    };
    virtual bool json_find_all(json_value& out_array, int skip = 0, int max = 10){
        auto conn = mongo->pool.acquire();
        auto coll = conn->database(database).collection(collection);
        std::vector<json_value> array = mongoose::find_all(coll, skip, max);
        json_value values;
        for(auto&& elem : array){
            mongoose::model::from_json_mongo_id(elem);
            values.push_back(elem);
        }
        out_array = values;
        return !out_array.empty();
    };
    virtual bool json_find_id(const std::string& id, json_value& out_data){
        auto conn = mongo->pool.acquire();
        auto coll = conn->database(database).collection(collection);
        auto value = mongoose::find_by_id(coll, id);
        if(!value) return false;
        out_data = value.value();
        mongoose::model::from_json_mongo_id(out_data);
        return true;
    };
    virtual bool json_create_id(const json_value& data, const std::string& id){
        auto conn = mongo->pool.acquire();
        auto coll = conn->database(database).collection(collection);
        return mongoose::insert_id(coll, id, data);
    };
    virtual bool json_create(const json_value& data, std::string& out_id){
        auto conn = mongo->pool.acquire();
        auto coll = conn->database(database).collection(collection);
        out_id = mongoose::insert(coll, data);
        return !out_id.empty();
    };
    virtual bool json_update(const std::string& id, json_value& data){
        auto conn = mongo->pool.acquire();
        auto coll = conn->database(database).collection(collection);
        return mongoose::update_id(coll, id, data);
    };
private:
    mongoose::mongodb* mongo;
    std::string database;
    std::string collection;
};

}

#endif
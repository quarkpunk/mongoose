#ifndef QUARKPUNK_MONGOOSE_REPOSITORY_HPP
#define QUARKPUNK_MONGOOSE_REPOSITORY_HPP

// this is an example of implementing a repository using mongoose
// you can use it for your needs or not - if you do not need all these methods
// this class does not use virtual methods because this is just an example
// if you need something like a CRUD repository and you will use all the methods
// that are declared here - then you can use it, but if you do not need all the methods
// then it is better to write your own repositories so as not to increase the process
// of template code generation in your application (if this is critical for you)

#include<mongoose/mongodb.hpp>
#include<mongoose/methods.hpp>

namespace mongoose{

template <typename T>
class repository{
public:
    repository(mongoose::mongodb* mongodb, const std::string database, const std::string collection){
        this->mongo = mongodb;
        this->database = database;
        this->collection = collection;
    }
    repository(const repository&) = delete;
    repository& operator=(const repository&) = delete;
// type <T> methods
public:
    bool find_all(std::vector<T>& results, const json_value& filter, int skip = 0, int max = 10){
        auto conn = mongo->pool.acquire();
        mongo_collection coll = conn->database(database).collection(collection);
        return mongoose::find_all(results, coll, filter, skip, max);
    }
    bool find_all(std::vector<T>& results, int skip = 0, int max = 10){
        auto conn = mongo->pool.acquire();
        mongo_collection coll = conn->database(database).collection(collection);
        return mongoose::find_all(results, coll, skip, max);
    }
    bool find_all(std::vector<T>& results, std::vector<std::string>& ids){
        auto conn = mongo->pool.acquire();
        mongo_collection coll = conn->database(database).collection(collection);
        return mongoose::find_all(results, coll, ids);
    }
    bool find_id(T& result, const std::string& id){
        auto conn = mongo->pool.acquire();
        mongo_collection coll = conn->database(database).collection(collection);
        return mongoose::find_id(result, coll, id);
    }
    bool find_one(T& result, const json_value& filter){
        auto conn = mongo->pool.acquire();
        mongo_collection coll = conn->database(database).collection(collection);
        return mongoose::find_one(result, coll, filter);
    }
    bool create_id(const T& data, const std::string& id){
        auto conn = mongo->pool.acquire();
        mongo_collection coll = conn->database(database).collection(collection);
        return mongoose::insert_id(data, coll, id);
    }
    bool create_many(const std::vector<T>& datas){
        auto conn = mongo->pool.acquire();
        mongo_collection coll = conn->database(database).collection(collection);
        return mongoose::insert_many(datas, coll);
    }
    bool create(const T& data, std::string& out_id){
        auto conn = mongo->pool.acquire();
        mongo_collection coll = conn->database(database).collection(collection);
        return mongoose::insert(data, coll, out_id);
    }
    bool update(const T& data, const std::string& id){
        auto conn = mongo->pool.acquire();
        mongo_collection coll = conn->database(database).collection(collection);
        return mongoose::update_id(data, coll, id);
    }
    bool remove(const std::string& id){
        auto conn = mongo->pool.acquire();
        mongo_collection coll = conn->database(database).collection(collection);
        return mongoose::remove_id(coll, id);
    }
// json methods
public:
    bool json_find_all(json_value& result, const json_value& filter, int skip = 0, int max = 10){
        auto conn = mongo->pool.acquire();
        mongo_collection coll = conn->database(database).collection(collection);
        return mongoose::json_find_all(result, coll, filter, skip, max);
    }
    bool json_find_all(json_value& result, int skip = 0, int max = 10){
        auto conn = mongo->pool.acquire();
        mongo_collection coll = conn->database(database).collection(collection);
        return mongoose::json_find_all(result, coll, skip, max);
    }
    bool json_find_id(json_value& result, const std::string& id){
        auto conn = mongo->pool.acquire();
        mongo_collection coll = conn->database(database).collection(collection);
        return mongoose::json_find_id(result, coll, id);
    }
    bool json_find_one(json_value& result, const json_value& filter){
        auto conn = mongo->pool.acquire();
        mongo_collection coll = conn->database(database).collection(collection);
        return mongoose::json_find_one(result, coll, filter);
    }
    bool json_create_id(const json_value& data, const std::string& id){
        auto conn = mongo->pool.acquire();
        mongo_collection coll = conn->database(database).collection(collection);
        return mongoose::json_insert_id(data, coll, id);
    }
    bool json_create(const json_value& data, std::string& out_id){
        auto conn = mongo->pool.acquire();
        auto coll = conn->database(database).collection(collection);
        return mongoose::json_insert(data, coll, out_id);
    }
    bool json_update(const json_value& data, const std::string& id){
        auto conn = mongo->pool.acquire();
        mongo_collection coll = conn->database(database).collection(collection);
        return mongoose::json_update_id(data, coll, id);
    }
protected:
    mongoose::mongodb* mongo;
public:
    std::string database;
    std::string collection;
};

}

#endif
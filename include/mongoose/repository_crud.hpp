#ifndef QUARKPUNK_MONGOOSE_REPOSITOY_CRUD_HPP
#define QUARKPUNK_MONGOOSE_REPOSITOY_CRUD_HPP

#include<optional>
#include<mongoose/repository.hpp>
#include<mongoose/types/oid.hpp>
#include<mongoose/json.hpp>
#include<bsoncxx/builder/stream/document.hpp>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;

namespace mongoose{

template<typename T>
class repository_crud : public repository{
public:
    repository_crud(mongoose::mongodb& mongo, const std::string& database, const std::string& collection)
    : repository(mongo, database, collection) {}
public:
    virtual std::optional<std::string> create_one(const T& model){
        auto conn = mongodb.pool.acquire();
        auto coll = conn->database(database).collection(collection);
        auto result = coll.insert_one(mongoose::json::to_bson_without_oid(model).view());
        if(result->result().inserted_count() == 0) return std::nullopt;
        return std::make_optional(result->inserted_id().get_oid().value.to_string());
    };
    virtual bool create_one(const std::string& oid, const T& model){
        auto conn = mongodb.pool.acquire();
        auto coll = conn->database(database).collection(collection);
        auto doc = document{}
            << "_id" << bsoncxx::oid{oid}
            << bsoncxx::builder::stream::concatenate(mongoose::json::to_bson_without_oid(model).view())
            << finalize;
        auto result = coll.insert_one(doc.view());
        return result->result().inserted_count() == 1;
    };
    virtual std::optional<T> find_by_id(const std::string& oid){
        auto conn = mongodb.pool.acquire();
        auto coll = conn->database(database).collection(collection);
        auto filter = document{} << "_id" << bsoncxx::oid{oid} << finalize;
        auto result = coll.find_one(filter.view());
        if(!result) return std::nullopt;
        T value_new;
        return mongoose::json::from_bson(value_new, result.value())
        ? std::make_optional<T>(value_new)
        : std::nullopt;
    };
    virtual bool update_one(const std::string& oid, const T& model){
        auto conn = mongodb.pool.acquire();
        auto coll = conn->database(database).collection(collection);
        auto filter = document{} << "_id" << bsoncxx::oid{oid} << finalize;
        auto doc = document{} << "$set" << mongoose::json::to_bson(model) << finalize;
        auto result = coll.update_one(filter.view(), doc.view());
        return result->modified_count() == 1 || result->upserted_count() == 1;
    };
    virtual bool delete_one(const std::string& oid){
        auto conn = mongodb.pool.acquire();
        auto coll = conn->database(database).collection(collection);
        auto filter = document{} << "_id" << bsoncxx::oid{oid} << finalize;
        auto result = coll.delete_one(filter.view());
        return result->deleted_count() == 1;
    }
};

}

#endif
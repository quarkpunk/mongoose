#include <mongoose/json.hpp>
#include <mongoose/logger.hpp>
#include <mongoose/bson.hpp>
#include <mongoose/types/date.hpp>
#include <mongoose/types/oid.hpp>
#include <mongoose/types/uuid.hpp>
#include <mongoose/mongodb.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

// if using nlohmann json
// nlohmann json macro for structs
#ifdef MONGOOSE_USE_NLOHMANN_JSON

#ifndef JSON_MODEL
#define JSON_MODEL NLOHMANN_DEFINE_TYPE_INTRUSIVE
#endif

#else

#ifndef JSON_MODEL
#define JSON_MODEL(Type, ...)
#endif

#endif

// test models
namespace models {

enum class product_type {
    NONE,
    CLOTH,
    FOOD
};

struct product_size {
    std::string name;
    std::string value;
    JSON_MODEL(product_size, name, value);
};

struct product {
    mongoose::oid _id;
    product_type type;
    std::string name;
    int price;
    std::vector<product_size> sizes;
    std::optional<mongoose::uuid> key;
    mongoose::date created_at;
    JSON_MODEL(product, _id, type, sizes, key, created_at);
};

}

std::optional<mongoose::oid> test_bson_insert_mongo(mongoose::mongodb& mongo){
    mongoose::logger::log(mongoose::logger::level::INFO, "--- test insert new document ---");

    // example
    models::product product_new;
    product_new.type = models::product_type::FOOD;
    product_new.name = "Pizza";
    product_new.price = 120;
    product_new.created_at = mongoose::types::date::now();
    product_new.key = mongoose::uuid();
    product_new.sizes = {
        models::product_size { .name="Small", .value="S" },
        models::product_size { .name="Medium", .value="M" }
    };

    // try get connection from pool
    const auto conn = mongo.pool.try_acquire();

    // no connection 
    if(!conn){
        mongoose::logger::log(mongoose::logger::level::ERROR,
            "failed get mongodb connection from pool, are you connected?"
        );
        return std::nullopt;
    }

    // get collection
    auto collection = conn.value()->database("mongoose").collection("test2");

    // try bson document building
    // and exclude '_id' field with oid
    // is recommended to create an Object ID on database side
    const auto bson_doc = mongoose::to_bson_exclude(product_new, "_id");

    // insert docuemt
    const auto result = collection.insert_one(bson_doc.view());

    // failed insert result
    if(!result){
        mongoose::logger::log(mongoose::logger::level::ERROR, "failed insert docuemnt");
        return std::nullopt;
    }

    // get and set new oid from mongodb
    product_new._id = result->inserted_id().get_oid().value;

    // log output
    mongoose::logger::log(mongoose::logger::level::INFO,
        "inserted new document: %s", product_new._id.to_string().c_str()
    );

    return std::make_optional(product_new._id);
}

std::optional<models::product> test_bson_from_mongo(mongoose::mongodb& mongo, const std::optional<mongoose::oid>& doc_oid){
    mongoose::logger::log(mongoose::logger::level::INFO, "--- test find exist document ---");

    // try get connection from pool
    const auto conn = mongo.pool.try_acquire();

    // no connection 
    if(!conn){
        mongoose::logger::log(mongoose::logger::level::ERROR,
            "failed get mongodb connection from pool, are you connected?"
        );
        return std::nullopt;
    }

    // get collection
    auto collection = conn.value()->database("mongoose").collection("test2");

    // make document filter
    const auto filter = make_document(
        kvp("_id", doc_oid.value())
    );

    // find query
    const auto result = collection.find_one(filter.view());

    // no result
    if(!result){
        mongoose::logger::log(mongoose::logger::level::ERROR, "failed find document");
        return std::nullopt;
    }

    // try parse model from bson
    try {
        // parse model
        models::product product_new = mongoose::from_bson<models::product>(result.value());
        return std::make_optional(product_new);
    }
    catch(const std::exception& e){
        // failed
        return std::nullopt;
    }
}

int main(int argc, char const *argv[]){
    // callback mongoose logger
    mongoose::logger::set_callback([](mongoose::logger::level level, const char* str){
        switch(level){
            case mongoose::logger::level::DEBUG: printf("debug: %s\n", str); break;
            case mongoose::logger::level::INFO: printf("%s\n", str); break;
            case mongoose::logger::level::WARN: printf("warn: %s\n", str); break;
            case mongoose::logger::level::ERROR: printf("error: %s\n", str); break;
        }
    });

    // try connect to mongodb
    mongoose::mongodb mongo("mongodb://root:password@localhost:27017");

    // methods
    std::optional<mongoose::oid> inserted_oid = test_bson_insert_mongo(mongo);
    std::optional<models::product> product_exist = test_bson_from_mongo(mongo, inserted_oid);

    // product no exist, exit
    if(!product_exist){
        return -1;
    }

    // convert to json string
    const std::string product_json = mongoose::json::to_string(*product_exist, 4);

    // log output
    mongoose::logger::log(mongoose::logger::level::INFO,
        "product (json): \n%s\n", product_json.c_str()
    );

    return 0;
}
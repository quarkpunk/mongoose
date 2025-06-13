#include<mongoose/mongodb.hpp>
#include<mongocxx/options/index.hpp>
#include<bsoncxx/builder/stream/document.hpp>
#include<mongocxx/instance.hpp>
#include<bsoncxx/json.hpp>
#include<mongocxx/client.hpp>
#include<stdio.h>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;
using bsoncxx::v_noabi::document::value;
using namespace mongoose;

static mongocxx::instance instance;

mongodb::~mongodb(){
    puts("mongodb: free");
}

void mongodb::ping() {
    auto client = pool.acquire();
    try {
        auto db = (*client)["admin"];
        auto ping_cmd = document{} << "ping" << 1 << finalize;
        auto result = db.run_command(ping_cmd.view());
        result.view()["ok"].get_double() == 1.0 
            ? puts("mongodb ping: ok") 
            : throw "mongodb ping: failed";
    } catch(const std::exception& e) {
        std::cerr << "mongodb ping: " << e.what() << std::endl;
    }
}

bool mongodb::index_create(const std::string& database, const std::string& collection, const index_value& index_def){
    try {
        auto conn = pool.acquire();
        auto coll = (*conn)[database][collection];
        auto keys = document{};
        for (const auto& [field, order] : index_def.fields){
            if (order == 0) {
                keys << field << "text";
            } else {
                keys << field << order;
            }
        }
        auto options = document{};
        if (!index_def.name.empty()){
            options << "name" << index_def.name;
        }
        if (index_def.unique){
            options << "unique" << *index_def.unique;
        }
        if (index_def.sparse){
            options << "sparse" << *index_def.sparse;
        }
        if (index_def.expire_after_seconds){
            options << "expireAfterSeconds" << *index_def.expire_after_seconds;
        }
        if (index_def.default_language){
            options << "default_language" << *index_def.default_language;
        }
        auto result = coll.create_index(keys.extract(),options.extract());
        return !result.view().empty();
    } catch (const std::exception& e) {
        printf("mongodb: (index_create) %s\n", e.what());
        return false;
    }
}

bool mongodb::index_drop(const std::string& database, const std::string& collection, const std::string& index_name) {
    try {
        auto conn = pool.acquire();
        auto coll = (*conn)[database][collection];
        coll.indexes().drop_one(index_name);
        printf("mongodb: index droped %s\n", index_name.c_str());
        return true;
    } catch (const std::exception& e) {
        printf("mongodb: (index_drop) %s\n", e.what());
        return false;
    }
}

std::vector<std::string> mongodb::index_list(const std::string& database, const std::string& collection) {
    std::vector<std::string> indexes;
    try {
        auto conn = pool.acquire();
        auto coll = (*conn)[database][collection];
        auto cursor = coll.list_indexes();
        for (auto&& index : cursor){
            if (auto name = index["name"]){
                indexes.push_back(name.get_string().value.to_string());
            }
        }
    } catch (const std::exception& e) {
        printf("mongodb: (index_list) %s\n", e.what());
    }
    return indexes;
}

void mongodb::index_list_print(const std::string& database, const std::string& collection){
    try{
        auto conn = pool.acquire();
        auto coll = (*conn)[database][collection];
        auto cursor = coll.list_indexes();
        std::cout << "mongodb: index list [" << database << "][" << collection << "]" << std::endl;
        for (auto&& index : cursor) {
            if (auto name = index["name"]) {
                std::cout << "- " << name.get_string().value.to_string();
                if (auto key = index["key"]) {
                    std::cout << " (fields: " << bsoncxx::to_json(key.get_document()) << ")";
                }
                std::vector<std::string> props;
                if (index["unique"]) props.push_back("unique");
                if (index["sparse"]) props.push_back("sparse");
                if (index["expireAfterSeconds"]) props.push_back("TTL");
                if (!props.empty()) {
                    std::cout << " [";
                    for (size_t i = 0; i < props.size(); ++i) {
                        if (i != 0) std::cout << ", ";
                        std::cout << props[i];
                    }
                    std::cout << "]";
                }
                std::cout << "\n";
            }
        }
    }
    catch(const std::exception& e){
        printf("mongodb: (index_list_print) %s\n", e.what());
    }
}
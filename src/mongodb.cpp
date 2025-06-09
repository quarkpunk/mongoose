#include<mongoose/mongodb.hpp>
#include<bsoncxx/builder/stream/document.hpp>
#include<mongocxx/instance.hpp>
#include <bsoncxx/json.hpp>
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
    } catch(const std::exception& e){
        std::cerr << "mongodb ping: " << e.what() << std::endl;
    }
}

value mongodb::create_objectid(std::string id){
    return make_document(kvp("_id", bsoncxx::oid{id}));
}

value mongodb::create_document(const std::string& oid, const std::string& json){
    return document{} << "_id"  << bsoncxx::oid{bsoncxx::stdx::string_view(oid.data())}
    << bsoncxx::builder::stream::concatenate(bsoncxx::from_json(json).view())
    << finalize;
}
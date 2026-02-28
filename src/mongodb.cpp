#include<mongoose/mongodb.hpp>
#include<mongoose/logger.hpp>
#include<mongocxx/options/index.hpp>
#include<mongocxx/instance.hpp>
#include<mongocxx/client.hpp>
#include<bsoncxx/builder/stream/document.hpp>
#include<bsoncxx/json.hpp>
#include<stdio.h>
using namespace mongoose;

static mongocxx::instance instance;

mongodb::~mongodb(){}

void mongodb::ping() {
    auto client = pool.acquire();
    try {
        auto db = (*client)["admin"];
        auto ping_cmd = bsoncxx::builder::stream::document{} << "ping" << 1 << bsoncxx::builder::stream::finalize;
        auto result = db.run_command(ping_cmd.view());
        result.view()["ok"].get_double() == 1.0 
            ? mongoose::logger::log(mongoose::logger::INFO, "mongodb connected") 
            : mongoose::logger::log(mongoose::logger::ERROR, "mongodb ping failed!");
    } catch(const std::exception& e) {
        mongoose::logger::log(mongoose::logger::ERROR, "mongodb ping -> %s", e.what());
    }
}
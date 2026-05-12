#include <iostream>
#include <nlohmann/json.hpp>
#include <mongoose/json.hpp>
#include <mongoose/logger.hpp>
#include <mongoose/bson.hpp>
#include <mongoose/types/date.hpp>
#include <mongoose/types/oid.hpp>

// nlohmann_json macros
#ifndef JSON_MODEL
#define JSON_MODEL NLOHMANN_DEFINE_TYPE_INTRUSIVE
#endif

// only for testing
// test models
namespace model {

struct post_stats {
    mongoose::date last_at;
    JSON_MODEL(post_stats, last_at);
};

struct post {
    mongoose::oid _id;
    std::optional<mongoose::oid> author_id;
    std::string text;
    post_stats stats;
    mongoose::date created_at;
    std::optional<mongoose::date> updated_at;
    std::optional<mongoose::date> deleted_at;
    JSON_MODEL(post, _id, author_id, text, stats, created_at, updated_at, deleted_at);
};

}

// only for testing
// test json raw text for parsing
constexpr const char* post_raw_json = R"({
    "_id": "68bf502bc6f9a9832f03ef01",
    "author_id": "60a9b0e0c5f1b2a3d4e5f680",
    "text": "any text string",
    "stats": {
        "last_at": 1772279920
    },
    "created_at": 1772279924,
    "updated_at": 1772280600,
    "deleted_at": null
})";

static void test_parse_from_json(){
    mongoose::logger::log(mongoose::logger::level::INFO, "[+] test parse_from_json");

    // parse to <T> model from JSON string
    // return value if success parsing
    // and cast BSON value to <T> 
    const std::optional<model::post> value = mongoose::json::from_string<model::post>(post_raw_json);

    // failed, null value
    if(!value){
        mongoose::logger::log(mongoose::logger::level::WARN, "failed parse test model user");
        return;
    }

    // success parsing
    // bson to json string for preview
    mongoose::logger::log(mongoose::logger::level::INFO, "[*] from JSON to BSON preview -> \n%s",
        bsoncxx::to_json(mongoose::to_bson(*value)).c_str()
    );
}

int main(int argc, char const *argv[]){
    // callback mongoose logger
    mongoose::logger::set_callback([](mongoose::logger::level level, const char* str){
        switch(level){
            case mongoose::logger::level::DEBUG: printf("debug: %s\n", str); return;
            case mongoose::logger::level::INFO: printf("%s\n", str); return;
            case mongoose::logger::level::WARN: printf("warn: %s\n", str); return;
            case mongoose::logger::level::ERROR: printf("error: %s\n", str); return;
        }
    });

    // test data with custom type date
    model::post post_new = {
        ._id = mongoose::oid{},
        .author_id = mongoose::oid{"60a9b0e0c5f1b2a3d4e5f680"},
        .text = "text",
        .stats = {
            .last_at = mongoose::date{}
        },
        .created_at = mongoose::types::date::from_timestamp(1772279924),
        .updated_at = mongoose::date{},
        .deleted_at = std::nullopt
    };

    // build BSON with custom types
    const auto doc = mongoose::to_bson(post_new);

    // bson to json string for preview
    mongoose::logger::log(mongoose::logger::level::INFO, "[*] BSON preview -> \n%s",
        bsoncxx::to_json(doc).c_str()
    );

    test_parse_from_json();

    mongoose::logger::log(mongoose::logger::level::INFO, "[*] STRUCT to JSON -> \n%s",
        mongoose::json::to_string(post_new).c_str()
    );

    /* nothing */
    return 0;
}
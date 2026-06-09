#include <mongoose/json.hpp>
#include <mongoose/logger.hpp>
#include <mongoose/bson.hpp>
#include <mongoose/types/date.hpp>
#include <mongoose/types/oid.hpp>

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

enum class user_gender {
    NONE,
    MAN,
    WOMAN
};

struct user_city {
    std::string id;
    std::string name;
    JSON_MODEL(user_city, id, name);
};

struct user_info {
    int age;
    std::string name;
    user_gender gender;
    std::optional<user_city> city;
    std::vector<mongoose::oid> tags;
    JSON_MODEL(user_info, age, name, gender, city, tags);
};

struct user {
    mongoose::oid _id;
    user_info info;
    mongoose::date created_at;
    mongoose::date updated_at;
    JSON_MODEL(user, _id, info, created_at, updated_at);
};

}

// json string for parsing
constexpr const char* json_user_string = R"({
    "_id": "68bf502bc6f9a9832f03ef01",
    "info": {
        "age": 27,
        "name": "miroslaw",
        "gender": 1,
        "city": null,
        "tags": [
            "60a9b0e0c5f1b2a3d4e5f680",
            "60a9b0e0c5f1b2a3d4e5f678"
        ]
    },
    "trash": "trash field",
    "created_at": 1645705800,
    "updated_at": 1778541821
})";

void test_parse_json(){
    mongoose::logger::log(mongoose::logger::level::INFO, "--- test parase json ---");

    // try parse model from json
    // used only json impl serialization
    const auto user = mongoose::json::from_string<models::user>(json_user_string);
    
    // user not value
    if(!user){
        mongoose::logger::log(mongoose::logger::level::INFO, "user null value, test stop");
        return;
    }

    // dump model to json
    const std::string user_dump = mongoose::json::to_string(user, 3);

    // log output
    mongoose::logger::log(mongoose::logger::level::INFO,
        "user json:\n%s\n", user_dump.c_str()
    );
}

void test_to_json(){
    mongoose::logger::log(mongoose::logger::level::INFO, "--- test dump json ---");
    
    // example
    models::user user_new = {
        ._id = mongoose::oid{"68bf502bc6f9a9832f03ef01"},
        .info = {
            .age = 27,
            .name = "Miroslaw",
            .gender = models::user_gender::MAN,
            .city = std::nullopt,
            .tags = {}
        },
        .created_at = mongoose::types::date::from_timestamp(1645705800),
        .updated_at = {}
    };

    // dynamic values
    user_new.info.tags.push_back(mongoose::oid{"60a9b0e0c5f1b2a3d4e5f680"});
    user_new.info.tags.push_back(mongoose::oid{"60a9b0e0c5f1b2a3d4e5f678"});
    user_new.updated_at = mongoose::types::date::now();

    // dump model to json
    const std::string user_dump = mongoose::json::to_string(user_new, 3);

    // log output
    mongoose::logger::log(mongoose::logger::level::INFO,
        "user json:\n%s\n", user_dump.c_str()
    );
}

void test_bson_to_json(){
    mongoose::logger::log(mongoose::logger::level::INFO, "--- test to bson (json preview) ---");

    // example
    models::user user_new = {
        ._id = mongoose::oid{"68bf502bc6f9a9832f03ef01"},
        .info = {
            .age = 27,
            .name = "Miroslaw",
            .gender = models::user_gender::MAN,
            .city = std::nullopt,
            .tags = {
                mongoose::oid{"60a9b0e0c5f1b2a3d4e5f680"},
                mongoose::oid{"60a9b0e0c5f1b2a3d4e5f678"}
            }
        },
        .created_at = mongoose::types::date::from_timestamp(1645705800),
        .updated_at = mongoose::types::date::now()
    };

    // try bson document building
    // and convert to JSON for preview
    const auto bson_doc = mongoose::to_bson(user_new);

    // if used json library
    #ifdef MONGOOSE_USE_NLOHMANN_JSON

    // json
    const auto json_doc = nlohmann::json::parse(bsoncxx::to_json(bson_doc));

    // log output
    mongoose::logger::log(mongoose::logger::level::INFO,
        "bson to json:\n%s\n", json_doc.dump(3).c_str()
    );

    #endif
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

    // methods
    test_parse_json();
    test_to_json();
    test_bson_to_json();

    return 0;
}
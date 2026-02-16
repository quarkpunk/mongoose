#include<mongoose/model.hpp>
#include<mongoose/types/date.hpp>
#include<mongoose/types/oid.hpp>
#include<mongoose/types/binary.hpp>

#include<bsoncxx/builder/basic/document.hpp>
#include<bsoncxx/types.hpp>
#include<bsoncxx/json.hpp>

enum class user_gender{
    NONE,
    MAN,
    WOMAN
};

struct user{
    mongoose::type::oid _id;
    std::string name;
    int number;
    user_gender gender;
    mongoose::type::date updated;
    mongoose::type::date created;
    MONGOOSE_MODEL(user, _id, name, number, gender, updated, created);
};

struct post{
    mongoose::type::oid _id;
    mongoose::type::binary text;
    mongoose::type::date created;
    MONGOOSE_MODEL(post, _id, text, created);
};

constexpr const char* json_string = R"({
    "_id": "68bf502bc6f9a9832f03ef01",
    "created": 1753280415,
    "name": "Angelina",
    "gender": 2,
    "number": 123,
    "updated": 1753776828
})";

static void mongoose_bson_test(){
    puts("🏷 test bson/model type");
    user model = {
        ._id = "68bf502bc6f9a9832f03ef01",
        .name = "Miroslaw",
        .number = 317,
        .gender = user_gender::MAN,
        .updated = mongoose::type::date{"2025-09-10T18:30:10.050Z"},
        .created = mongoose::type::date{"2025-05-08T12:05:10.000Z"}
    };

    // build to bsoncxx value
    const bsoncxx::document::value model_bson = mongoose::model::to_bson(model);

    // debug print all fields
    for (const bsoncxx::document::element& value : model_bson){
        printf(" * bson-field: %s\n", value.key().data());
    }
    
    // model from bson value/view
    user user_from_bson;
    if(!mongoose::model::from_bson(user_from_bson, model_bson)){
        puts(" [!] failed from_bson, bad format");
        return;
    }

    printf(" [OK] to bson string: %s\n", nlohmann::json(model).dump().c_str());
    printf(" [OK] to json string: %s\n", mongoose::model::to_string(user_from_bson).c_str());
}

static void mongoose_bson_from_json(){
    puts("🏷 test string/model type");
    user model;
    if(!mongoose::model::from_string(model, json_string)){
        puts(" [!] failed from_string, bad format");
    }

    printf(" [OK] to bson string: %s\n", nlohmann::json(model).dump().c_str());
    printf(" [OK] to json string: %s\n", mongoose::model::to_string(model).c_str());
}

static void mongoose_binary(){
    puts("🏷 binary type");
    post post = {
        ._id = "68bf502bc6f9a9832f03ef01",
        .text = mongoose::type::binary{"MongoDB Extended JSON"}
    };
    printf(" [OK] to bson string: %s\n", nlohmann::json(post).dump().c_str());
    printf(" [OK] to json string: %s\n", mongoose::model::to_string(post).c_str());
}

int main(int argc, char const *argv[]){
    mongoose_bson_test();
    mongoose_bson_from_json();
    mongoose_binary();
    return 0;
}
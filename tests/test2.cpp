#include<mongoose/json.hpp>
#include<mongoose/types/date.hpp>
#include<mongoose/types/oid.hpp>

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
    MODEL_JSON(user, _id, name, number, gender, updated, created);
};

constexpr const char* json_string = R"({
    "_id": "68bf502bc6f9a9832f03ef01",
    "created": 1753280415,
    "name": "Angelina",
    "gender": 2,
    "number": 123,
    "updated": 1753776828
})";

static void mongoose_model_test(){
    puts("🏷 test json/bson model type");
    user model = {
        ._id = "68bf502bc6f9a9832f03ef01",
        .name = "Miroslaw",
        .number = 317,
        .gender = user_gender::MAN,
        .updated = mongoose::type::date{"2025-09-10T18:30:10.050Z"},
        .created = mongoose::type::date{"2025-05-08T12:05:10.000Z"}
    };
    const auto model_bson = mongoose::json::to_bson(model);
    const std::string model_json = mongoose::json::to_string(model);
    const std::string model_bson_example = bsoncxx::to_json(model_bson);
    printf(" ├─ model export (json to string): %s\n", model_json.c_str());
    printf(" ├─ model export (bson to string): %s\n", model_bson_example.c_str());

    user model_from_bson;
    if(!mongoose::json::from_bson(model_from_bson, model_bson)){
        puts(" [!] failed from_bson, bad format");
        return;
    }

    user model_from_string;
    if(!mongoose::json::from_string(model_from_string, json_string)){
        puts(" [!] failed from_string, bad format");
        return;
    }

    printf(" ├─ model import (json from bson): %s\n", mongoose::json::to_string(model_from_bson).c_str());
    printf(" └─ model import (json from string): %s\n", mongoose::json::to_string(model_from_string).c_str());
}

int main(int argc, char const *argv[]){
    mongoose_model_test();
    return 0;
}
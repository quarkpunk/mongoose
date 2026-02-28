#include<mongoose/bson.hpp>
#include<iostream>

// only for testing
// to make working with dates easier
using time_point = std::chrono::system_clock::time_point;

// only for testing
// test models
namespace model{

struct user_city{
    std::string id;
    std::string name;
};
struct user_info{
    int age;
    std::string name;
    std::optional<user_city> city;
    std::vector<bsoncxx::oid> tags;
};
struct user{
    bsoncxx::oid _id;
    user_info info;
    time_point created_at;
    time_point updated_at;
};

}

// only for testing
// test json raw text for parsing
constexpr const char* user_raw_json = R"({
    "_id": "68bf502bc6f9a9832f03ef01",
    "info": {
        "age": 27,
        "name": "miroslaw",
        "city": null,
        "tags": [
            "60a9b0e0c5f1b2a3d4e5f680",
            "60a9b0e0c5f1b2a3d4e5f678"
        ]
    },
    "trash": "this trash field, for test",
    "created_at": 1772279924,
    "updated_at": 1772280600
})";



// only for testing
// fast convert/cast to milliseconds (timestamp)
static inline const time_point date_to_bson(int32_t time_ms){
    std::chrono::milliseconds duration_ms(time_ms);
    return std::chrono::time_point<std::chrono::system_clock>(duration_ms);
}

// only for testing
// fast convert/cast to milliseconds (timestamp)
static inline const int32_t date_parse_bson(time_point time){
    return std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()).count();
}

// only for testing
// preview method for testing
static void print_struct_values(const model::user& user_value){
    mongoose::logger::log(mongoose::logger::level::INFO, "[*] C++ struct preview from BSON");
    std::cout
        << "_id: " << user_value._id.to_string() << "\n"
        << "info: {\n"
        << "    age: " << user_value.info.age << "\n"
        << "    name: " << user_value.info.name.c_str() << "\n"
        << "    city: " << "nullptr" << "\n"
        << "    tags: [\n";
        for(const auto value : user_value.info.tags){
            std::cout << "        " << value.to_string().c_str() << "\n";
        }
        std::cout << "    ]\n"
        << "    created_at: " <<  date_parse_bson(user_value.created_at) << "\n"
        << "    updated_at: " <<  date_parse_bson(user_value.updated_at) << "\n"
    << "}\n";
}

static void test_parse_from_json(){
    mongoose::logger::log(mongoose::logger::level::INFO, "[+] test parse_from_json");

    // parse to <T> model from JSON string
    // return value if success parsing
    // and cast BSON value to <T> 
    const std::optional<model::user> value = mongoose::from_json<model::user>(user_raw_json);

    // failed, null value
    if(!value){
        mongoose::logger::log(mongoose::logger::level::WARN, "failed parse test model user");
        return;
    }

    // success parsing
    // bson to json string for preview
    mongoose::logger::log(mongoose::logger::level::INFO, "[*] BSON preview -> \n%s",
        bsoncxx::to_json(mongoose::to_bson(*value)).c_str()
    );

    // get value
    const model::user& user_value = *value;

    // print debug real values
    print_struct_values(user_value);
}

static void test_to_bson(){
    mongoose::logger::log(mongoose::logger::level::INFO, "[+] test to_bson");

    // test model
    model::user user_value = {
        ._id = bsoncxx::oid{"68bf502bc6f9a9832f03ef01"},
        .info = {
            .age = 27,
            .name = "miroslaw",
            .city = std::nullopt,
            .tags = {
                bsoncxx::oid{"60a9b0e0c5f1b2a3d4e5f680"},
                bsoncxx::oid{"60a9b0e0c5f1b2a3d4e5f678"}
            }
        },
        .created_at = date_to_bson(1772279924),
        .updated_at = date_to_bson(1772280600)
    };

    // parse C++ struct to BSON document value
    auto doc = mongoose::to_bson(user_value);

    // bson to json string for preview
    mongoose::logger::log(mongoose::logger::level::INFO, "[*] BSON preview -> \n%s",
        bsoncxx::to_json(doc).c_str()
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

    // parsing test
    test_parse_from_json();

    // build bson test
    test_to_bson();

    // done
    mongoose::logger::log(mongoose::logger::level::INFO, "[OK] tests done!");

    return 0;
}
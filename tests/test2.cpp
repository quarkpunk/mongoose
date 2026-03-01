#include<mongoose/bson.hpp>
#include<iostream>

// only for testing
// custom type for models
namespace type{

// only for testing
// to make working with dates easier
using time_point = std::chrono::system_clock::time_point;

struct date{
private:
    time_point time_;
public:
    explicit date(time_point tp) : time_(tp) {}
    date() : time_(std::chrono::system_clock::now()) {}
    date(int64_t milliseconds) : time_(time_point(std::chrono::milliseconds(milliseconds))) {}
    time_point time() const { return time_; }
public:
    bool operator==(const date& other) const { return time_ == other.time_; }
    bool operator!=(const date& other) const { return !(*this == other); }
    bool operator<(const date& other) const { return time_ < other.time_; }
    bool operator>(const date& other) const { return time_ > other.time_; }
    bool operator==(const time_point& tp) const { return time_ == tp; }
    bool operator!=(const time_point& tp) const { return !(*this == tp); }
public:
    int64_t to_timestamp() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(time_.time_since_epoch()).count();
    }
    time_point to_timepoint() const {
        return time_;
    }
    date add_days(int days) const {
        return date(time_ + std::chrono::hours(24 * days));
    }
    date add_hours(int hours) const {
        return date(time_ + std::chrono::hours(hours));
    }
    date add_minutes(int minutes) const {
        return date(time_ + std::chrono::minutes(minutes));
    }
    date add_seconds(int seconds) const {
        return date(time_ + std::chrono::seconds(seconds));
    }
    date sub_days(int days) const {
        return date(time_ - std::chrono::hours(24 * days));
    }
    date sub_hours(int hours) const {
        return date(time_ - std::chrono::hours(hours));
    }
    date sub_minutes(int minutes) const {
        return date(time_ - std::chrono::minutes(minutes));
    }
    date sub_seconds(int seconds) const {
        return date(time_ - std::chrono::seconds(seconds));
    }
};

}

namespace mongoose::traits{

// macro registrate new
// custom type for mongoose
template<>
struct is_custom_serializable<type::date> : std::true_type {};

// template for custom type<type::date>
template<>
bsoncxx::document::value serialize_custom(const type::date& value) {
    auto builder = bsoncxx::builder::stream::document{};
    builder << "$date" << value.to_timestamp();
    return builder << bsoncxx::builder::stream::finalize;
}

// template for custom type<type::date>
template<>
type::date deserialize_custom(const bsoncxx::document::view& doc, std::string_view name) {
    const auto element = doc[name.data()];
    if(!element){
        return type::date{};
    }
    switch(element.type()) {
        case bsoncxx::type::k_int32: return type::date{element.get_int32().value};
        case bsoncxx::type::k_int64: return type::date{element.get_int64().value};
        case bsoncxx::type::k_date:  return type::date{element.get_date().to_int64()};
        default: return type::date{};
    }
}

}

// only for testing
// test models
namespace model{

struct post_stats{
    type::date last_at;
};

struct post{
    bsoncxx::oid _id;
    bsoncxx::oid author_id;
    std::string text;
    post_stats stats;
    type::date created_at;
    type::date updated_at;
    std::optional<type::date> deleted_at;
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
    const std::optional<model::post> value = mongoose::from_json<model::post>(post_raw_json);

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
        ._id = bsoncxx::oid{},
        .author_id = bsoncxx::oid{"60a9b0e0c5f1b2a3d4e5f680"},
        .text = "text",
        .stats = {
            .last_at = type::date{}
        },
        .created_at = type::date{1772279924},
        .updated_at = type::date{},
        .deleted_at = std::nullopt
    };

    // build BSON with custom types
    const auto doc = mongoose::to_bson(post_new);

    // bson to json string for preview
    mongoose::logger::log(mongoose::logger::level::INFO, "[*] BSON preview -> \n%s",
        bsoncxx::to_json(doc).c_str()
    );

    test_parse_from_json();

    /* nothing */
    return 0;
}
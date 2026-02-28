# Mongoose (MongoDB C++)
Simple and efficient C++ serializer/deserializer for BSON documents intended for MongoDB. The library implements work with (almost) all the basic types necessary for working with BSON and JSON structures

✅ Convenient API
- Easy to use
- Models/Schemas

📦 Dependencies
- C++20
- CMake
- mongocxx (4.0.0)
- boost.pfr (from mongocxx)

## Types Serialization from JSON and BSON
These are the main data types this library can work with, specifically parsing and converting data between C++ and the BSON data format. Unfortunately, mongoose doesn't have its own JSON module for converting native BSON types into JSON string

| Type | BSON | JSON | About |
|---|---|---|---|
| int 32/64     | ✅    | ✅    | numbers|
| bool          | ✅    | ✅    | boolean |
| float         | ✅    | ✅    | num float |
| double        | ✅    | ✅    | num double |
| string        | ✅    | ✅    | basic string |
| array         | ✅    | ✅    | fixed array |
| vector        | ✅    | ✅    | dynamic vector |
| enum class    | ✅    | ✅    | enum class type |
| optional      | ✅    | ✅    | value or null |
| object        | ✅    | ✅    | nested object |
| time_point    | ✅    | ✅    | date time value |
| object_oid    | ✅    | ✅    | id object |
| binary        | ...   | ...   | binary data |

## How to use
Download the library in any way and place it in your project directory, include it in your CMake project
```cmake
# add library
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/mongoose)

# link library
target_link_libraries(${PROJECT_NAME} PRIVATE
    # dont link mongo::mongocxx_static
    # mongoose alredy link mongocxx lib
    mongoose
)
```

## Model/Schema
Models are created quite simply, it is enough to declare structures in the usual C++ form, and that's all

```cpp
// to make working with dates easier
using time_point = std::chrono::system_clock::time_point;

// nested model/document
struct user_city{
    std::string id;
    std::string name;
};
// nested model/document
struct user_info{
    int age;
    std::string name;
    std::optional<user_city> city;
    std::vector<bsoncxx::oid> tags;
};
// main user model/document
struct user{
    bsoncxx::oid _id;
    user_info info;
    time_point created_at;
    time_point updated_at;
};
```
To serialize your data into a BSON document, you need to
```cpp
// build BSON document value from struct
auto doc = mongoose::to_bson(user_value);
```
```cpp
// build BSON document without _id
// if you need to insert/update the entire document
auto doc = mongoose::to_bson_without_id(user_value);
auto result = collection.insert_one(doc.view());
```

## JSON
To deserialize from JSON to BSON, for example, to retrieve from your frontend, use the `mongoose::from_json<T>(string)` method. The result will be an optional value `std::optional<T>`. If the JSON string is invalid, a `null` result will be returned
```cpp
// parse to <T> model from JSON string
// return value if success parsing
// and cast BSON value to <T> 
std::optional<user> value = mongoose::from_json<user>(json_string);

// check optional
// failed, null value
if(!value){
    return;
}
```

To serialize pure JSON from your C++ struct, it's best to assemble the JSON manually using any C++ JSON library, such as `nlohmann_json`, this is a simplified code example
```cpp
// build handmade your JSON
nlohmann::json user_json = {
    { "id", user._id },
    { "info", {
        { "age", user.info.age },
        { "name", user.info.name },
        { "city", nullptr },
        { "tags", nlohmann::json::array() },
    }},
    { "updated_at", user.updated_at },    
    { "created_at", user.created_at },    
}

// export JSON to string
std::string result = user_json.dump();
```

As a last resort, you can use the native `bsoncxx::to_json` method from the bsoncxx library to construct a JSON string from your BSON document
```cpp
// parse <T> model to BSON document value
auto doc = mongoose::to_bson(user_value);
// bsoncxx native method
// build JSON string from BSON document
std::string result = bsoncxx::to_json(doc);
```

However, there's one important caveat. Your JSON will contain native BSON variable types, such as `$oid`, `$date`, and others. If this is acceptable to you, and the purity of your JSON isn't critical, then you can safely use `bsoncxx::to_json` method

## Concept
In fact, I was tired of constantly writing the same code, so I wanted to template it and make it more accessible for writing my backend web applications without problems, I was inspired by the mongoose library in javascript and decided to write my own small version
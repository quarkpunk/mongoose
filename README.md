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

| C++           | BSON |  About |
|---------------|------|---------|
| int 32/64     | ✅   | numbers|
| bool          | ✅   | boolean |
| float         | ✅   | num float |
| double        | ✅   | num double |
| string        | ✅   | basic string |
| array         | ✅   | fixed array |
| vector        | ✅   | dynamic vector |
| enum class    | ✅   | enum class type |
| optional      | ✅   | value or null |
| struct        | ✅   | nested object |
| date          | ✅   | time point value |
| object id     | ✅   | bson object id |
| uuid 4/7      | ✅   | binary uuid types |
| binary        | ...  | binary data |
| variant       | ...  | variant value |

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
// using date type
#include <mongoose/types/oid.hpp>
#include <mongoose/types/date.hpp>

// nested document
struct user_info {
    int age;
    std::string name;
    std::string city;
    std::vector<mongoose::oid> tags;
};

// main user document
struct user {
    std::optional<mongoose::oid> _id;
    std::optional<user_info> info;
    mongoose::date created_at;
};
```
To serialize your data into a BSON document, you need to
```cpp
// build BSON document value from struct
auto doc = mongoose::to_bson(user_value);
```
```cpp
// build BSON document without _id field
// if you need to insert/update the entire document
auto doc = mongoose::to_bson_exclude(user_value, "_id");
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

## Concept
In fact, I was tired of constantly writing the same code, so I wanted to template it and make it more accessible for writing my backend web applications without problems, I was inspired by the mongoose library in javascript and decided to write my own small version
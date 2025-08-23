# Mongoose (MongoDB C++)

A simple and efficient C++ client for MongoDB, inspired by the Mongoose JavaScript library. A simple and efficient C++ client for MongoDB, built on top of the Mongoose JavaScript library. The library templates basic CRUD methods for working with data and simplifies interaction with MongoDB

✅ Convenient API
- Easy to use
- Models/Schemas
- Templated repository

📦 Dependencies
- C++17
- CMake
- mongocxx (3.8.1)
- nlohmann/json (3.11)

## Repository
Mongoose implements a basic CRUD repository that allows you to work with data at the model level <T> and raw JSON, the repository was created to template the main methods, but be careful if you care about memory optimization, as this is a template CRUD class that does not have virtual methods

```cpp
// mongodb instance
mongoose::mongodb mongo("connection stirng");
// template repositories
mongoose::repository_crud<model::auth> auth_repos(&mongo, "db", "auths");
mongoose::repository_crud<model::user> user_repos(&mongo, "db", "users");
// use example
model::auth auth_found;
// return true if success
auth_repos.find_by_id("56e6835a90ffdc6014c728c0", auth_found);
```

## Model/Schema
Models are created quite simply, all you need is to declare your structures and put define `MODEL_JSON` in them, then specify the name of your structure as the first argument, and then list the other fields, all this is necessary to enable `serialization` and `deserialization` between JSON/C++/BSON
```cpp
// other model
struct user_image{
    std::string original_url;
    std::string croped_url;
    MODEL_JSON(user_image, original_url, croped_url);
};
// main model
struct user{
    mongoose::type::oid _id;
    std::string name;
    std::string descr;
    user_image avatar;
    MODEL_JSON(user, _id, name, descr, avatar);
};
```

I would like to pay special attention to the `id` field, which should not be included for `MODEL_JSON`, this is due to the fact that the `serializer` itself will insert the ObjectID value from the MongoDB document, but only if this is the root document where there is an ObjectID

from json string to <T> your model
```cpp
// any input json string
// http body for example 
std::string json_str;
// your model
user user_model;
// return true if json model valid or false
mongoose::json::from_string(user_model, json_str);
```

from model <T> to string json
```cpp
// your model
user user_model;
// return final json string
std::string result = mongoose::json::to_string(user_model);
```

## JSON
`nlohmann json` was chosen due to its maximum flexible functionality, including the need for auto serialization/deserialization, so far this is the only library that is suitable for these tasks. At the moment, the name `json_value` is used for `nlohmann::json`, since many people like me may have conflicts of json names within one project, if you do not like the name `json_value` you can declare any of your own, this is normal


## Concept
In fact, I was tired of constantly writing the same code, so I wanted to template it and make it more accessible for writing my backend web applications without problems, I was inspired by the mongoose library in javascript and decided to write my own small version
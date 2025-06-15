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


## Namespaces
Description of the main methods of mongoose namespaces
| namespace           | description                         |
| ------------------- | ----------------------------------- |
| mongoose            | main template methods for you       |
| mongoose::base      | based methods of mongodb            |
| mongoose::session   | methods for working with sessions   |
| mongoose::model     | methods for working with models     |
| mongoose::traits    | traits of the main model transformations |


## Repository
Mongoose implements a basic CRUD repository that allows you to work with data at the model level <T> and raw JSON, the repository was created to template the main methods, but be careful if you care about memory optimization, as this is a template CRUD class that does not have virtual methods

```cpp
// mongodb instance
mongoose::mongodb mongo("connection stirng");
// template repositories
mongoose::repository<model::auth> auth_repos(&mongo, "db", "auths");
mongoose::repository<model::user> user_repos(&mongo, "db", "users");
// use example
model::auth auth_found;
// return true if success
auth_repos.find_id("56e6835a90ffdc6014c728c0", auth_found);
```
If the repository seems large to you, you can simply copy everything from `repository.hpp` and implement it as you wish in your header file using the templated pre-made methods


## Model/Schema
Models are created quite simply, all you need is to declare your structures and put define `JSON_MODEL` in them, then specify the name of your structure as the first argument, and then list the other fields, all this is necessary to enable `serialization` and `deserialization` between JSON/C++/BSON
```cpp
// other model
struct user_image{
    std::string original_url;
    std::string croped_url;
    JSON_MODEL(user_image, original_url, croped_url);
};
// main model
struct user{
    std::string id;
    std::string name;
    std::string descr;
    user_image avatar;
    JSON_MODEL(user, name, descr, avatar);
};
```

I would like to pay special attention to the `id` field, which should not be included for `JSON_MODEL`, this is due to the fact that the `serializer` itself will insert the ObjectID value from the MongoDB document, but only if this is the root document where there is an ObjectID

from json string to <T> your model
```cpp
// any input json string
// http body for example 
std::string json_str;
// your model
user user_model;
// return true if json model valid or false
mongoose::model::from_string(user_model, json_str);
```

from model <T> to string json
```cpp
// your model
user user_model;
// return final json string
std::string result = mongoose::model::to_string(user_model);
```


## Serialization/Deserialization
If you want to return the model as a string along with the id field, for example to your frontend, here is an example of how to do it
```cpp
// your any model
user user_model;
// return string and insert into json 'id' value
mongoose::model::to_string_with_id(user_model);
```
If you want to handle json models from MongoDB yourself, you can use the `from_string_with_id` function, this function will `deserialize` the json into your model and automatically embed the ObjectID into your `id` field (if it exists in your structure)
```cpp
// mongodb result
// use bson to json 
std::string json_str;
// your any model
user user_model;
// return true of false, and insert 'id' value into model
mongoose::model::from_string_with_id(user_model, json_str);
```


## Indexes
Create, delete indexes, example of basic methods
```cpp
// default index
mongoose::index_value idx1;
idx1.fields = {{"email", 1}};
idx1.unique = true;
mongo.index_create("mydb", "users", idx1);

// composite index
mongoose::index_value idx2;
idx2.fields = {{"last_name", 1}, {"created_at", -1}};
idx2.name = "name_created_idx";
mongo.index_create("mydb", "users", idx2);

// ttl index
mongoose::index_value ttl_idx;
ttl_idx.fields = {{"last_activity", 1}};
ttl_idx.expire_after_seconds = 86400;
mongo.index_create("mydb", "sessions", ttl_idx);

// text index
mongoose::index_value text_idx;
text_idx.fields = {{"content", 0}};
text_idx.default_language = "russian";
mongo.index_create("mydb", "articles", text_idx);
```
Getting a list of indexes
```cpp
// return index names vector<string>
mongo.index_list("database", "collection");
// outputs only the list to the terminal
// more detailed information
mongo.index_list_print("database", "collection");
```


## JSON
`nlohmann json` was chosen due to its maximum flexible functionality, including the need for auto serialization/deserialization, so far this is the only library that is suitable for these tasks. At the moment, the name `json_value` is used for `nlohmann::json`, since many people like me may have conflicts of json names within one project, if you do not like the name `json_value` you can declare any of your own, this is normal


## Concept
In fact, I was tired of constantly writing the same code, so I wanted to template it and make it more accessible for writing my backend web applications without problems, I was inspired by the mongoose library in javascript and decided to write my own small version
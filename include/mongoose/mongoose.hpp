#ifndef QUARKPUNK_MONGOOSE_HPP
#define QUARKPUNK_MONGOOSE_HPP

#include<mongocxx/collection.hpp>
#include<nlohmann/json.hpp>
#include<vector>
#include<optional>

using json_value = nlohmann::json;

namespace mongoose{
    std::optional<json_value> find_one(mongocxx::collection& collection, const json_value& filter);
    std::string insert(mongocxx::collection& collection, const json_value& document);
    bool insert_id(mongocxx::collection& collection, const std::string& id, const json_value& document);
    bool update(mongocxx::collection& collection, const json_value& filter, const json_value& update_data, bool upsert = false);
    bool remove(mongocxx::collection& collection, const json_value& filter);
    std::vector<json_value> find_all(mongocxx::collection& collection, const json_value& filter = json_value::object(), int skip = 0, int limit = 10);
    std::vector<json_value> find_all(mongocxx::collection& collection, int skip = 0, int limit = 20);
    bool update_id(mongocxx::collection& collection, const std::string& id, const json_value& update_data, bool upsert = false);
    bool remove_id(mongocxx::collection& collection, const std::string& id);
    std::optional<json_value> find_by_id(mongocxx::collection& collection, const std::string& id);
};

#endif
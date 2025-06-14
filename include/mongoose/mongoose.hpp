#ifndef QUARKPUNK_MONGOOSE_HPP
#define QUARKPUNK_MONGOOSE_HPP

#include<mongocxx/collection.hpp>
#include<nlohmann/json.hpp>
#include<vector>
#include<optional>

using json_value = nlohmann::json;
using mongo_collection = mongocxx::collection;
using mongo_session = mongocxx::v_noabi::client_session;

// default methods
namespace mongoose::base{
    std::optional<json_value> find_one(mongo_collection& collection, const json_value& filter);
    std::vector<json_value> find_all(mongo_collection& collection, const json_value& filter = json_value::object(), int skip = 0, int limit = 10);
    std::vector<json_value> find_all(mongo_collection& collection, int skip = 0, int limit = 20);
    std::optional<json_value> find_by_id(mongo_collection& collection, const std::string& id);
    std::string insert(mongo_collection& collection, const json_value& document);
    bool insert_id(mongo_collection& collection, const std::string& id, const json_value& document);
    bool update(mongo_collection& collection, const json_value& filter, const json_value& update_data, bool upsert = false);
    bool update_id(mongo_collection& collection, const std::string& id, const json_value& update_data, bool upsert = false);
    bool update_id_raw(mongo_collection& collection, const std::string& id, const json_value& document_json, bool upsert = false);
    bool remove(mongo_collection& collection, const json_value& filter);
    bool remove_id(mongo_collection& collection, const std::string& id);
}

// session methods
namespace mongoose::session{
    std::optional<json_value> find_one(mongo_session& session, mongo_collection& collection, const json_value& filter);
    std::vector<json_value> find_all(mongo_session& session, mongo_collection& collection, const json_value& filter = json_value::object(), int skip = 0, int limit = 10);
    std::vector<json_value> find_all(mongo_session& session, mongo_collection& collection, int skip = 0, int limit = 20);
    std::optional<json_value> find_by_id(mongo_session& session, mongo_collection& collection, const std::string& id);
    std::string insert(mongo_session& session, mongo_collection& collection, const json_value& document);
    bool insert_id(mongo_session& session, mongo_collection& collection, const std::string& id, const json_value& document);
    bool update(mongo_session& session, mongo_collection& collection, const json_value& filter, const json_value& update_data, bool upsert = false);
    bool update_id(mongo_session& session, mongo_collection& collection, const std::string& id, const json_value& update_data, bool upsert = false);
    bool update_id_raw(mongo_session& session, mongo_collection& collection, const std::string& id, const json_value& document_json, bool upsert = false);
    bool remove(mongo_session& session, mongo_collection& collection, const json_value& filter);
    bool remove_id(mongo_session& session, mongo_collection& collection, const std::string& id);
}

#endif
#ifndef QUARKPUNK_MONGOOSE_HPP
#define QUARKPUNK_MONGOOSE_HPP

#include<mongocxx/bulk_write.hpp>
#include<mongocxx/collection.hpp>
#include<nlohmann/json.hpp>
#include<vector>
#include<optional>

using json_value = nlohmann::json;
using mongo_collection = mongocxx::collection;
using mongo_session = mongocxx::v_noabi::client_session;
using bulk_write = mongocxx::v_noabi::bulk_write;
using view_or_value = bsoncxx::v_noabi::document::view_or_value;
using options_find = mongocxx::options::find;
using options_update = mongocxx::options::update;
using options_insert = mongocxx::options::insert;
using options_delete = mongocxx::options::delete_options;

// utils methods
namespace mongoose::utils{
    bool objectid_is_valid(const std::string& id);
    bool objectid_is_valid_strict(const std::string& id);
}

// default methods
namespace mongoose::base{
    // default
    std::optional<json_value> find_one(mongo_collection& collection, const json_value& filter);
    std::vector<json_value> find_all(mongo_collection& collection, const json_value& filter = json_value::object(), const options_find& option_find = options_find());
    std::vector<json_value> find_all(mongo_collection& collection, const json_value& filter = json_value::object(), int skip = 0, int limit = 10);
    std::vector<json_value> find_all(mongo_collection& collection, std::vector<std::string>& ids, int skip = 0, int limit = 10);
    std::vector<json_value> find_all(mongo_collection& collection, int skip = 0, int limit = 20);
    std::optional<json_value> find_id(mongo_collection& collection, const std::string& id);
    std::string insert(mongo_collection& collection, const json_value& document);
    bool update(mongo_collection& collection, const json_value& filter, const json_value& update_data, bool upsert = false);
    bool remove(mongo_collection& collection, const json_value& filter);
    bool insert_many(mongo_collection& collection, const std::vector<json_value>& datas);
    bool insert_many(mongo_collection& collection, const std::vector<json_value>& datas, std::vector<std::string>& return_oids);
    bool insert_id(mongo_collection& collection, const std::string& id, const json_value& document);
    bool update_id(mongo_collection& collection, const std::string& id, const json_value& update_data, bool upsert = false);
    bool remove_id(mongo_collection& collection, const std::string& id);
    // session
    std::optional<json_value> find_one(mongo_session& session, mongo_collection& collection, const json_value& filter);
    std::vector<json_value> find_all(mongo_session& session, mongo_collection& collection, const json_value& filter = json_value::object(), const options_find& option_find = options_find());
    std::vector<json_value> find_all(mongo_session& session, mongo_collection& collection, const json_value& filter = json_value::object(), int skip = 0, int limit = 10);
    std::vector<json_value> find_all(mongo_session& session, mongo_collection& collection, std::vector<std::string>& ids, int skip = 0, int limit = 10);
    std::vector<json_value> find_all(mongo_session& session, mongo_collection& collection, int skip = 0, int limit = 20);
    std::optional<json_value> find_id(mongo_session& session, mongo_collection& collection, const std::string& id);
    std::string insert(mongo_session& session, mongo_collection& collection, const json_value& document);
    bool update(mongo_session& session, mongo_collection& collection, const json_value& filter, const json_value& update_data, bool upsert = false);
    bool remove(mongo_session& session, mongo_collection& collection, const json_value& filter);
    bool insert_many(mongo_session& session, mongo_collection& collection, const std::vector<json_value>& datas);
    bool insert_many(mongo_session& session, mongo_collection& collection, const std::vector<json_value>& datas, std::vector<std::string>& return_oids);
    bool insert_id(mongo_session& session, mongo_collection& collection, const std::string& id, const json_value& document);
    bool update_id(mongo_session& session, mongo_collection& collection, const std::string& id, const json_value& update_data, bool upsert = false);
    bool remove_id(mongo_session& session, mongo_collection& collection, const std::string& id);
}

// raw methods
namespace mongoose::raw{
    // default
    std::optional<json_value> find_one(mongo_collection& collection, const view_or_value& filter, const options_find& option_find = options_find());
    std::vector<json_value> find_all(mongo_collection& collection, const view_or_value& filter, const options_find& option_find = options_find());
    std::string insert(mongo_collection& collection, const view_or_value& value, const options_insert& option_insert = options_insert());
    bool update(mongo_collection& collection, const view_or_value& filter, const view_or_value& value, const options_update& option_update = options_update());
    bool remove(mongo_collection& collection, const view_or_value& filter, const options_delete& options_delete = options_delete());
    bool insert_id(mongo_collection& collection, const std::string& id, const view_or_value& value, const options_insert& option_insert = options_insert());
    bool update_id(mongo_collection& collection, const std::string& id, const view_or_value& value, const options_update& option_update = options_update());
    bool remove_id(mongo_collection& collection, const std::string& id, const options_delete& options_delete = options_delete());
    // sessions
    std::optional<json_value> find_one(mongo_session& session, mongo_collection& collection, const view_or_value& filter, const options_find& option_find = options_find());
    std::vector<json_value> find_all(mongo_session& session, mongo_collection& collection, const view_or_value& filter, const options_find& option_find = options_find());
    std::string insert(mongo_session& session, mongo_collection& collection, const view_or_value& value, const options_insert& option_insert = options_insert());
    bool update(mongo_session& session, mongo_collection& collection, const view_or_value& filter, const view_or_value& value, const options_update& option_update = options_update());
    bool remove(mongo_session& session, mongo_collection& collection, const view_or_value& filter, const options_delete& options_delete = options_delete());
    bool insert_id(mongo_session& session, mongo_collection& collection, const std::string& id, const view_or_value& value, const options_insert& option_insert = options_insert());
    bool update_id(mongo_session& session, mongo_collection& collection, const std::string& id, const view_or_value& value, const options_update& option_update = options_update());
    bool remove_id(mongo_session& session, mongo_collection& collection, const std::string& id, const options_delete& options_delete = options_delete());
}

// bulk methods
namespace mongoose::base{
    void bulk_insert_append(bulk_write& bulk, const std::vector<json_value>& inserts);
    void bulk_update_append(bulk_write& bulk, const std::vector<std::pair<std::string, json_value>>& updates);
    void bulk_remove_append(bulk_write& bulk, const std::vector<std::string>& removes_id);
}

#endif
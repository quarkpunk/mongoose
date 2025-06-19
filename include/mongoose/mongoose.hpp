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

// default methods
namespace mongoose::base{
    std::optional<json_value> find_one(mongo_collection& collection, const json_value& filter);
    std::vector<json_value> find_all(mongo_collection& collection, const json_value& filter = json_value::object(), int skip = 0, int limit = 10);
    std::vector<json_value> find_all(mongo_collection& collection, std::vector<std::string>& ids, int skip = 0, int limit = 99);
    std::vector<json_value> find_all(mongo_collection& collection, int skip = 0, int limit = 20);
    std::optional<json_value> find_by_id(mongo_collection& collection, const std::string& id);
    std::string insert(mongo_collection& collection, const json_value& document);
    bool insert_many(mongo_collection& collection, const std::vector<json_value>& datas);
    bool insert_many(mongo_collection& collection, const std::vector<json_value>& datas, std::vector<std::string>& return_oids);
    bool insert_id(mongo_collection& collection, const std::string& id, const json_value& document);
    bool update(mongo_collection& collection, const json_value& filter, const json_value& update_data, bool upsert = false);
    bool update_id(mongo_collection& collection, const std::string& id, const json_value& update_data, bool upsert = false);
    bool update_id_raw(mongo_collection& collection, const std::string& id, const json_value& document_json, bool upsert = false);
    bool remove(mongo_collection& collection, const json_value& filter);
    bool remove_id(mongo_collection& collection, const std::string& id);
}

// bulk methods
namespace mongoose::base{
    void bulk_insert_append(bulk_write& bulk, const std::vector<json_value>& inserts);
    void bulk_update_append(bulk_write& bulk, const std::vector<std::pair<std::string, json_value>>& updates);
    void bulk_remove_append(bulk_write& bulk, const std::vector<std::string>& removes_id);
}

// session methods
namespace mongoose::session{
    std::optional<json_value> find_one(mongo_session& session, mongo_collection& collection, const json_value& filter);
    std::vector<json_value> find_all(mongo_session& session, mongo_collection& collection, const json_value& filter = json_value::object(), int skip = 0, int limit = 10);
    std::vector<json_value> find_all(mongo_session& session, mongo_collection& collection, int skip = 0, int limit = 20);
    std::optional<json_value> find_by_id(mongo_session& session, mongo_collection& collection, const std::string& id);
    std::string insert(mongo_session& session, mongo_collection& collection, const json_value& document);
    bool insert_many(mongo_session& session, mongo_collection& collection, const std::vector<json_value>& datas);
    bool insert_many(mongo_session& session, mongo_collection& collection, const std::vector<json_value>& datas, std::vector<std::string>& return_oids);
    bool insert_id(mongo_session& session, mongo_collection& collection, const std::string& id, const json_value& document);
    bool update(mongo_session& session, mongo_collection& collection, const json_value& filter, const json_value& update_data, bool upsert = false);
    bool update_id(mongo_session& session, mongo_collection& collection, const std::string& id, const json_value& update_data, bool upsert = false);
    bool update_id_raw(mongo_session& session, mongo_collection& collection, const std::string& id, const json_value& document_json, bool upsert = false);
    bool remove(mongo_session& session, mongo_collection& collection, const json_value& filter);
    bool remove_id(mongo_session& session, mongo_collection& collection, const std::string& id);
}

#endif
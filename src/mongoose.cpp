#include<mongoose/mongoose.hpp>
#include<mongoose/model.hpp>
#include<bsoncxx/builder/stream/document.hpp>
#include<mongocxx/client.hpp>
#include<bsoncxx/validate.hpp>
#include<bsoncxx/oid.hpp>
#include<bsoncxx/json.hpp>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::finalize;

// utils methods

static inline bsoncxx::document::value to_bson(const json_value& j){
    return bsoncxx::from_json(j.dump());
}

static inline json_value to_json(const bsoncxx::document::view& view){
    return json_value::parse(bsoncxx::to_json(view));
}

bool mongoose::utils::objectid_is_valid(const std::string& id){
    if(id.empty()) return false;
    return id.length() == 24;
}

bool mongoose::utils::objectid_is_valid_strict(const std::string& id){
    if(id.empty()) return false;
    try { bsoncxx::oid oid{id}; return true; }
    catch (const std::exception& e) { return false; }
}

// default methods

std::optional<json_value> mongoose::base::find_one(mongo_collection& collection, const json_value& filter) {
    auto result = collection.find_one(to_bson(filter).view());
    return result ? std::make_optional(to_json(*result)) : std::nullopt;
}

std::vector<json_value> mongoose::base::find_all(mongo_collection& collection, const json_value& filter, const options_find& options){
    auto cursor = collection.find(to_bson(filter).view(), options);
    std::vector<json_value> results;
    for(auto&& doc : cursor){
        results.push_back(to_json(doc));
    }
    return results;
}

std::vector<json_value> mongoose::base::find_all(mongo_collection& collection, const json_value& filter, int skip, int limit) {
    auto options = mongocxx::v_noabi::options::find();
    options.skip(skip);
    options.limit(limit);
    auto cursor = collection.find(to_bson(filter).view(), options);
    std::vector<json_value> results;
    for(auto&& doc : cursor){
        results.push_back(to_json(doc));
    }
    return results;
}

std::vector<json_value> mongoose::base::find_all(mongo_collection& collection, int skip, int limit){
    auto options = mongocxx::v_noabi::options::find();
    options.skip(skip);
    options.limit(limit);
    auto cursor = collection.find({}, options);
    std::vector<json_value> results;
    for(auto&& doc : cursor){
        results.push_back(to_json(doc));
    }
    return results;
}

std::vector<json_value> mongoose::base::find_all(mongo_collection& collection, std::vector<std::string>& ids, int skip, int limit){
    auto ids_array = bsoncxx::builder::basic::array{};
    for(const auto& id : ids){
        ids_array.append(bsoncxx::oid(id));
    }
    auto filter = document{} << "_id" << open_document << "$in" << ids_array << close_document << finalize;
    auto options = mongocxx::v_noabi::options::find();
    options.skip(skip);
    options.limit(limit);
    auto cursor = collection.find(filter.view(), options);
    std::vector<json_value> results;
    for(auto&& doc : cursor){
        results.push_back(to_json(doc));
    }
    return results;
}

std::optional<json_value> mongoose::base::find_id(mongo_collection& collection, const std::string& id) {
    bsoncxx::oid oid{id};
    auto filter = document{} << "_id" << oid << finalize;
    auto result = collection.find_one(filter.view());
    return result ? std::make_optional(to_json(*result)) : std::nullopt;
}

std::string mongoose::base::insert(mongo_collection& collection, const json_value& document) {
    auto result = collection.insert_one(to_bson(document).view());
    return result ? result->inserted_id().get_oid().value.to_string() : std::string{};
}

bool mongoose::base::insert_many(mongo_collection& collection, const std::vector<json_value>& datas){
    std::vector<bsoncxx::document::value> documents;
    for(const auto& val : datas){
        documents.push_back(to_bson(val));
    }
    auto result = collection.insert_many(documents);
    return result && result->result().inserted_count() > 0;
}

bool mongoose::base::insert_many(mongo_collection& collection, const std::vector<json_value>& datas, std::vector<std::string>& return_oids){
    std::vector<bsoncxx::document::value> documents;
    for(const auto& val : datas){
        documents.push_back(to_bson(val));
    }
    auto result = collection.insert_many(documents);
    if(result && result->result().inserted_count() > 0){
        for(const auto& val : result->inserted_ids()){
            return_oids.push_back(val.second.get_oid().value.to_string());
        }
        return true;
    }
    return false;
}

bool mongoose::base::insert_id(mongo_collection& collection, const std::string& id, const json_value& doc){
    auto query = document{} << "_id"  << bsoncxx::oid{id}
    << bsoncxx::builder::stream::concatenate(to_bson(doc))
    << finalize;
    auto result = collection.insert_one(query.view());
    return result->result().inserted_count() > 0;
}

bool mongoose::base::update(mongo_collection& collection, const json_value& filter, const json_value& update_data, bool upsert) {
    auto options = mongocxx::options::update().upsert(upsert);
    document update_doc;
    update_doc << "$set" << to_bson(update_data);
    auto result = collection.update_one(to_bson(filter).view(),update_doc.view(),options);
    return result && (result->modified_count() > 0 || result->matched_count() > 0);
}

bool mongoose::base::update_id(mongo_collection& collection, const std::string& id, const json_value& update_data, bool upsert) {
    bsoncxx::oid oid{id};
    auto filter = document{} << "_id" << oid << finalize;
    auto update_doc = document{}  << "$set" << to_bson(update_data) << finalize;
    auto options = mongocxx::options::update().upsert(upsert);
    auto result = collection.update_one(filter.view(), update_doc.view(),options);
    return result && (result->modified_count() > 0 || result->matched_count() > 0);
}

bool mongoose::base::remove(mongo_collection& collection, const json_value& filter) {
    auto result = collection.delete_one(to_bson(filter).view());
    return result && result->deleted_count() > 0;
}

bool mongoose::base::remove_id(mongo_collection& collection, const std::string& id) {
    bsoncxx::oid oid{id};
    auto filter = document{} << "_id" << oid << finalize;
    auto result = collection.delete_one(filter.view());
    return result && result->deleted_count() > 0;
}

// session methods

std::optional<json_value> mongoose::base::find_one(mongo_session& session, mongo_collection& collection, const json_value& filter) {
    auto result = collection.find_one(session, to_bson(filter).view());
    return result ? std::make_optional(to_json(*result)) : std::nullopt;
}

std::vector<json_value> mongoose::base::find_all(mongo_session& session, mongo_collection& collection, const json_value& filter, const options_find& options){
    auto cursor = collection.find(session, to_bson(filter).view(), options);
    std::vector<json_value> results;
    for(auto&& doc : cursor){
        results.push_back(to_json(doc));
    }
    return results;
}

std::vector<json_value> mongoose::base::find_all(mongo_session& session, mongo_collection& collection, const json_value& filter, int skip, int limit){
    auto options = mongocxx::v_noabi::options::find();
    options.skip(skip);
    options.limit(limit);
    auto cursor = collection.find(session, to_bson(filter).view(), options);
    std::vector<json_value> results;
    for(auto&& doc : cursor){
        results.push_back(to_json(doc));
    }
    return results;
}

std::vector<json_value> mongoose::base::find_all(mongo_session& session, mongo_collection& collection, int skip, int limit){
    auto options = mongocxx::v_noabi::options::find();
    options.skip(skip);
    options.limit(limit);
    auto cursor = collection.find(session, {}, options);
    std::vector<json_value> results;
    for(auto&& doc : cursor){
        results.push_back(to_json(doc));
    }
    return results;
}

std::vector<json_value> mongoose::base::find_all(mongo_session& session, mongo_collection& collection, std::vector<std::string>& ids, int skip, int limit){
    auto ids_array = bsoncxx::builder::basic::array{};
    for(const auto& id : ids){
        ids_array.append(bsoncxx::oid(id));
    }
    auto filter = document{} << "_id" << open_document << "$in" << ids_array << close_document << finalize;
    auto options = mongocxx::v_noabi::options::find();
    options.skip(skip);
    options.limit(limit);
    auto cursor = collection.find(filter.view(), options);
    std::vector<json_value> results;
    for(auto&& doc : cursor){
        results.push_back(to_json(doc));
    }
    return results;
}

std::optional<json_value> mongoose::base::find_id(mongo_session& session, mongo_collection& collection, const std::string& id) {
    bsoncxx::oid oid{id};
    auto filter = document{} << "_id" << oid << finalize;
    auto result = collection.find_one(session, filter.view());
    return result ? std::make_optional(to_json(*result)) : std::nullopt;
}

std::string mongoose::base::insert(mongo_session& session, mongo_collection& collection, const json_value& document) {
    auto result = collection.insert_one(session, to_bson(document).view());
    return result ? result->inserted_id().get_oid().value.to_string() : std::string{};
}

bool mongoose::base::insert_many(mongo_session& session, mongo_collection& collection, const std::vector<json_value>& datas){
    std::vector<bsoncxx::document::value> documents;
    for(const auto& val : datas){
        documents.push_back(to_bson(val));
    }
    auto result = collection.insert_many(session, documents);
    return result && result->result().inserted_count() > 0;
}

bool mongoose::base::insert_many(mongo_session& session, mongo_collection& collection, const std::vector<json_value>& datas, std::vector<std::string>& return_oids){
    std::vector<bsoncxx::document::value> documents;
    for(const auto& val : datas){
        documents.push_back(to_bson(val));
    }
    auto result = collection.insert_many(session, documents);
    if(result && result->result().inserted_count() > 0){
        for(const auto& val : result->inserted_ids()){
            return_oids.push_back(val.second.get_oid().value.to_string());
        }
        return true;
    }
    return false;
}

bool mongoose::base::insert_id(mongo_session& session, mongo_collection& collection, const std::string& id, const json_value& doc){
    auto query = document{} << "_id"  << bsoncxx::oid{id}
    << bsoncxx::builder::stream::concatenate(to_bson(doc))
    << finalize;
    auto result = collection.insert_one(session, query.view());
    return result->result().inserted_count() > 0;
}

bool mongoose::base::update(mongo_session& session, mongo_collection& collection, const json_value& filter, const json_value& update_data, bool upsert) {
    auto options = mongocxx::options::update().upsert(upsert);
    document update_doc;
    update_doc << "$set" << to_bson(update_data);
    auto result = collection.update_one(session, to_bson(filter).view(),update_doc.view(),options);
    return result && (result->modified_count() > 0 || result->matched_count() > 0);
}

bool mongoose::base::update_id(mongo_session& session, mongo_collection& collection, const std::string& id, const json_value& update_data, bool upsert) {
    bsoncxx::oid oid{id};
    auto filter = document{} << "_id" << oid << finalize;
    auto update_doc = document{}  << "$set" << to_bson(update_data) << finalize;
    auto options = mongocxx::options::update().upsert(upsert);
    auto result = collection.update_one(session, filter.view(), update_doc.view(),options);
    return result && (result->modified_count() > 0 || result->matched_count() > 0);
}

bool mongoose::base::remove(mongo_session& session, mongo_collection& collection, const json_value& filter) {
    auto result = collection.delete_one(session, to_bson(filter).view());
    return result && result->deleted_count() > 0;
}

bool mongoose::base::remove_id(mongo_session& session, mongo_collection& collection, const std::string& id) {
    bsoncxx::oid oid{id};
    auto filter = document{} << "_id" << oid << finalize;
    auto result = collection.delete_one(session, filter.view());
    return result && result->deleted_count() > 0;
}

// raw methods

std::optional<json_value> mongoose::raw::find_one(mongo_collection& collection, const view_or_value& filter, const options_find& option_find) {
    auto result = collection.find_one(filter, option_find);
    return result ? std::make_optional(to_json(*result)) : std::nullopt;
}

std::vector<json_value> mongoose::raw::find_all(mongo_collection& collection, const view_or_value& filter, const options_find& option_find){
    auto cursor = collection.find(filter, option_find);
    std::vector<json_value> results;
    for(auto&& doc : cursor){
        results.push_back(to_json(doc));
    }
    return results;
}

std::string mongoose::raw::insert(mongo_collection& collection, const view_or_value& value, const options_insert& option_insert) {
    auto result = collection.insert_one(value.view(), option_insert);
    return result ? result->inserted_id().get_oid().value.to_string() : std::string{};
}

bool mongoose::raw::update(mongo_collection& collection, const view_or_value& filter, const view_or_value& value, const options_update& option_update){
    auto result = collection.update_one(filter.view(), value, option_update);
    return result && (result->modified_count() > 0 || result->matched_count() > 0);
}

bool mongoose::raw::remove(mongo_collection& collection, const view_or_value& filter, const options_delete& options_delete){
    auto result = collection.delete_one(filter, options_delete);
    return result && result->deleted_count() > 0;
}

bool mongoose::raw::insert_id(mongo_collection& collection, const std::string& id, const view_or_value& value, const options_insert& option_insert){
    auto query = document{} << "_id"  << bsoncxx::oid{id} << bsoncxx::builder::stream::concatenate(value) << finalize;
    auto result = collection.insert_one(query.view(), option_insert);
    return result->result().inserted_count() > 0;
}

bool mongoose::raw::update_id(mongo_collection& collection, const std::string& id, const view_or_value& value, const options_update& option_update){
    auto filter = document{} << "_id" << bsoncxx::oid{id} << finalize;
    auto result = collection.update_one(filter.view(), value, option_update);
    return result && (result->modified_count() > 0 || result->matched_count() > 0);
}

bool mongoose::raw::remove_id(mongo_collection& collection, const std::string& id, const options_delete& options_delete){
    auto filter = document{} << "_id" << bsoncxx::oid{id} << finalize;
    auto result = collection.delete_one(filter.view(), options_delete);
    return result && result->deleted_count() > 0;
}

// raw session methods

std::optional<json_value> mongoose::raw::find_one(mongo_session& session, mongo_collection& collection, const view_or_value& filter, const options_find& option_find) {
    auto result = collection.find_one(session, filter, option_find);
    return result ? std::make_optional(to_json(*result)) : std::nullopt;
}

std::vector<json_value> mongoose::raw::find_all(mongo_session& session, mongo_collection& collection, const view_or_value& filter, const options_find& option_find){
    auto cursor = collection.find(session, filter, option_find);
    std::vector<json_value> results;
    for(auto&& doc : cursor){
        results.push_back(to_json(doc));
    }
    return results;
}

std::string mongoose::raw::insert(mongo_session& session, mongo_collection& collection, const view_or_value& value, const options_insert& option_insert) {
    auto result = collection.insert_one(session, value.view(), option_insert);
    return result ? result->inserted_id().get_oid().value.to_string() : std::string{};
}

bool mongoose::raw::update(mongo_session& session, mongo_collection& collection, const view_or_value& filter, const view_or_value& value, const options_update& option_update){
    auto result = collection.update_one(session, filter.view(), value, option_update);
    return result && (result->modified_count() > 0 || result->matched_count() > 0);
}

bool mongoose::raw::remove(mongo_session& session, mongo_collection& collection, const view_or_value& filter, const options_delete& options_delete){
    auto result = collection.delete_one(session, filter, options_delete);
    return result && result->deleted_count() > 0;
}

bool mongoose::raw::insert_id(mongo_session& session, mongo_collection& collection, const std::string& id, const view_or_value& value, const options_insert& option_insert){
    auto query = document{} << "_id"  << bsoncxx::oid{id} << bsoncxx::builder::stream::concatenate(value) << finalize;
    auto result = collection.insert_one(session, query.view(), option_insert);
    return result->result().inserted_count() > 0;
}

bool mongoose::raw::update_id(mongo_session& session, mongo_collection& collection, const std::string& id, const view_or_value& value, const options_update& option_update){
    auto filter = document{} << "_id" << bsoncxx::oid{id} << finalize;
    auto result = collection.update_one(session, filter.view(), value, option_update);
    return result && (result->modified_count() > 0 || result->matched_count() > 0);
}

bool mongoose::raw::remove_id(mongo_session& session, mongo_collection& collection, const std::string& id, const options_delete& options_delete){
    auto filter = document{} << "_id" << bsoncxx::oid{id} << finalize;
    auto result = collection.delete_one(session, filter.view(), options_delete);
    return result && result->deleted_count() > 0;
}

// bulk methods

void mongoose::base::bulk_insert_append(bulk_write& bulk, const std::vector<json_value>& inserts){
    for(const auto& json_data : inserts){
        bulk.append(mongocxx::model::insert_one(to_bson(json_data).view()));
    }
}

void mongoose::base::bulk_update_append(bulk_write& bulk, const std::vector<std::pair<std::string, json_value>>& updates){
    for(const auto& [id, json_data] : updates){
        if(id.empty()) continue;
        auto filter = document{} << "_id" << bsoncxx::oid(id) << finalize;
        auto update = document{} << "$set" << to_bson(json_data) << finalize;
        bulk.append(mongocxx::model::update_one(filter.view(), update.view()));
    }
}

void mongoose::base::bulk_remove_append(bulk_write& bulk, const std::vector<std::string>& removes_id){
    for(const auto& id : removes_id){
        auto filter = document{} << "_id" << bsoncxx::oid(id) << finalize;
        bulk.append(mongocxx::model::delete_one(filter.view()));
    }
}
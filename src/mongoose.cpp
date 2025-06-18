#include<mongoose/mongoose.hpp>
#include<mongoose/model.hpp>
#include<bsoncxx/builder/stream/document.hpp>
#include<mongocxx/client.hpp>
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

// default methods

std::optional<json_value> mongoose::base::find_one(mongo_collection& collection, const json_value& filter) {
    auto result = collection.find_one(to_bson(filter).view());
    return result ? std::make_optional(to_json(*result)) : std::nullopt;
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

std::optional<json_value> mongoose::base::find_by_id(mongo_collection& collection, const std::string& id) {
    bsoncxx::oid oid{id};
    auto filter = document{} << "_id" << oid << finalize;
    auto result = collection.find_one(filter.view());
    return result ? std::make_optional(to_json(*result)) : std::nullopt;
}

std::string mongoose::base::insert(mongo_collection& collection, const json_value& document) {
    auto result = collection.insert_one(to_bson(document).view());
    return result ? result->inserted_id().get_oid().value.to_string() : std::string{};
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
    return result && (result->modified_count() > 0 || result->upserted_id());
}

bool mongoose::base::update_id(mongo_collection& collection, const std::string& id, const json_value& update_data, bool upsert) {
    bsoncxx::oid oid{id};
    auto filter = document{} << "_id" << oid << finalize;
    auto update_doc = document{}  << "$set" << to_bson(update_data) << finalize;
    auto options = mongocxx::options::update().upsert(upsert);
    auto result = collection.update_one(filter.view(), update_doc.view(),options);
    return result && (result->modified_count() > 0 || result->upserted_id());
}

bool mongoose::base::update_id_raw(mongo_collection& collection, const std::string& id, const json_value& document_json, bool upsert) {
    bsoncxx::oid oid{id};
    auto filter = document{} << "_id" << oid << finalize;
    auto options = mongocxx::options::update().upsert(upsert);
    auto result = collection.update_one(filter.view(), to_bson(document_json), options);
    return result && (result->modified_count() > 0 || result->upserted_id());
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

// bulk methods

bool mongoose::base::insert_bulk(mongo_collection& collection, const std::vector<json_value>& documents_json){
    std::vector<bsoncxx::document::value> documents;
    for(const auto& doc_json : documents_json){
        documents.push_back(to_bson(doc_json));
    }
    if(documents.empty()) return false;
    auto result = collection.insert_many(documents);
    return result && result.value().inserted_count() > 0;
}

bool mongoose::base::insert_bulk(mongo_collection& collection, const std::vector<json_value>& documents_json, std::vector<std::string>& return_oids){
    std::vector<bsoncxx::document::value> documents;
    for(const auto& doc_json : documents_json){
        documents.push_back(to_bson(doc_json));
    }
    if(documents.empty()) return false;
    auto result = collection.insert_many(documents);

    if(result && result.value().inserted_count() == 0){
        return false;
    }
    std::vector<std::string> result_ids;
    for(const auto& pair : result.value().inserted_ids()) {
        const bsoncxx::v_noabi::document::element id_value = pair.second;
        if (id_value.type() == bsoncxx::type::k_oid) {
            result_ids.push_back(id_value.get_oid().value.to_string());
        }
        else if (id_value.type() == bsoncxx::type::k_utf8) {
            result_ids.push_back(id_value.get_string().value.to_string());
        }
    }
    return true;
}

bool mongoose::base::update_bulk(mongo_collection& collection, const std::vector<std::string>& ids, const json_value& document_json){
    std::vector<mongocxx::model::update_many> updates;
    auto update_doc = to_bson(document_json);
    for(const auto& id : ids){
        auto filter = document{} << "_id" << id << finalize;
        updates.emplace_back(filter.view(), update_doc.view());
    }
    if(updates.empty()) return false;
    auto result = collection.bulk_write(updates);
    return result && result.value().modified_count() > 0;
}

bool mongoose::base::update_bulk(mongo_collection& collection, const std::vector<std::pair<std::string, json_value>>& updates){
    std::vector<mongocxx::model::update_one> bulk_updates;
    for(const auto& [id, json_data] : updates) {
        auto filter = document{} << "_id" << bsoncxx::oid(id) << finalize;
        auto update = document{} << "$set" << to_bson(json_data) << finalize;
        bulk_updates.emplace_back(filter.view(), update.view());
    }
    if(bulk_updates.empty()) {
        return false;
    }
    auto result = collection.bulk_write(bulk_updates);
    return result && result->modified_count() > 0;
}

bool mongoose::base::update_bulk_raw(mongo_collection& collection, const std::vector<std::pair<std::string, json_value>>& updates){
    std::vector<mongocxx::model::update_one> bulk_updates;
    for(const auto& [id, json_data] : updates) {
        auto filter = document{} << "_id" << bsoncxx::oid(id) << finalize;
        bulk_updates.emplace_back(filter.view(), to_bson(json_data).view());
    }
    if(bulk_updates.empty()) {
        return false;
    }
    auto result = collection.bulk_write(bulk_updates);
    return result && result->modified_count() > 0;
}

bool mongoose::base::remove_bulk(mongo_collection& collection, const std::vector<std::string>& ids){
    if(ids.empty()) return false;
    auto filter = document{};
    filter << "_id" << open_document << "$in" << [&ids](auto&& arr) {
        for (const auto& id : ids) {
            arr << id;
        }
    } << close_document;
    auto result = collection.delete_many(filter.view());
    return result && result.value().deleted_count() > 0;
}

// session methods

std::optional<json_value> mongoose::session::find_one(mongo_session& session, mongo_collection& collection, const json_value& filter) {
    auto result = collection.find_one(session, to_bson(filter).view());
    return result ? std::make_optional(to_json(*result)) : std::nullopt;
}

std::vector<json_value> mongoose::session::find_all(mongo_session& session, mongo_collection& collection, const json_value& filter, int skip, int limit) {
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

std::vector<json_value> mongoose::session::find_all(mongo_session& session, mongo_collection& collection, int skip, int limit){
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

std::optional<json_value> mongoose::session::find_by_id(mongo_session& session, mongo_collection& collection, const std::string& id) {
    bsoncxx::oid oid{id};
    auto filter = document{} << "_id" << oid << finalize;
    auto result = collection.find_one(session, filter.view());
    return result ? std::make_optional(to_json(*result)) : std::nullopt;
}

std::string mongoose::session::insert(mongo_session& session, mongo_collection& collection, const json_value& document) {
    auto result = collection.insert_one(session, to_bson(document).view());
    return result ? result->inserted_id().get_oid().value.to_string() : std::string{};
}

bool mongoose::session::insert_id(mongo_session& session, mongo_collection& collection, const std::string& id, const json_value& doc){
    auto query = document{} << "_id"  << bsoncxx::oid{id}
    << bsoncxx::builder::stream::concatenate(to_bson(doc))
    << finalize;
    auto result = collection.insert_one(session, query.view());
    return result->result().inserted_count() > 0;
}

bool mongoose::session::update(mongo_session& session, mongo_collection& collection, const json_value& filter, const json_value& update_data, bool upsert) {
    auto options = mongocxx::options::update().upsert(upsert);
    document update_doc;
    update_doc << "$set" << to_bson(update_data);
    auto result = collection.update_one(session, to_bson(filter).view(),update_doc.view(),options);
    return result && (result->modified_count() > 0 || result->upserted_id());
}

bool mongoose::session::update_id(mongo_session& session, mongo_collection& collection, const std::string& id, const json_value& update_data, bool upsert) {
    bsoncxx::oid oid{id};
    auto filter = document{} << "_id" << oid << finalize;
    auto update_doc = document{}  << "$set" << to_bson(update_data) << finalize;
    auto options = mongocxx::options::update().upsert(upsert);
    auto result = collection.update_one(session, filter.view(), update_doc.view(),options);
    return result && (result->modified_count() > 0 || result->upserted_id());
}

bool mongoose::session::update_id_raw(mongo_session& session, mongo_collection& collection, const std::string& id, const json_value& document_json, bool upsert) {
    bsoncxx::oid oid{id};
    auto filter = document{} << "_id" << oid << finalize;
    auto options = mongocxx::options::update().upsert(upsert);
    auto result = collection.update_one(session, filter.view(), to_bson(document_json), options);
    return result && (result->modified_count() > 0 || result->upserted_id());
}

bool mongoose::session::remove(mongo_session& session, mongo_collection& collection, const json_value& filter) {
    auto result = collection.delete_one(session, to_bson(filter).view());
    return result && result->deleted_count() > 0;
}

bool mongoose::session::remove_id(mongo_session& session, mongo_collection& collection, const std::string& id) {
    bsoncxx::oid oid{id};
    auto filter = document{} << "_id" << oid << finalize;
    auto result = collection.delete_one(session, filter.view());
    return result && result->deleted_count() > 0;
}
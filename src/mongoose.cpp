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
using namespace mongoose;

static bsoncxx::document::value to_bson(const json_value& j){
    return bsoncxx::from_json(j.dump());
}

static json_value to_json(const bsoncxx::document::view& view){
    return json_value::parse(bsoncxx::to_json(view));
}

std::optional<json_value> mongoose::find(mongocxx::collection& collection, const json_value& filter) {
    auto result = collection.find_one(to_bson(filter).view());
    return result ? std::make_optional(to_json(*result)) : std::nullopt;
}

std::string mongoose::insert(mongocxx::collection& collection, const json_value& document) {
    auto result = collection.insert_one(to_bson(document).view());
    return result ? result->inserted_id().get_oid().value.to_string() : std::string{};
}

bool mongoose::insert_id(mongocxx::collection& collection, const std::string& id, const json_value& doc){
    auto query = document{} << "_id"  << bsoncxx::oid{id}
    << bsoncxx::builder::stream::concatenate(to_bson(doc))
    << finalize;
    auto result = collection.insert_one(query.view());
    return result->result().inserted_count() > 0;
}

bool mongoose::update(mongocxx::collection& collection, const json_value& filter, const json_value& update_data, bool upsert) {
    auto options = mongocxx::options::update().upsert(upsert);
    document update_doc;
    update_doc << "$set" << to_bson(update_data);
    auto result = collection.update_one(to_bson(filter).view(),update_doc.view(),options);
    return result && (result->modified_count() > 0 || result->upserted_id());
}

bool mongoose::remove(mongocxx::collection& collection, const json_value& filter) {
    auto result = collection.delete_one(to_bson(filter).view());
    return result && result->deleted_count() > 0;
}

std::optional<json_value> mongoose::find_one(mongocxx::collection& collection, const json_value& filter) {
    return find(collection, filter);
}

std::vector<json_value> mongoose::find_all(mongocxx::collection& collection, const json_value& filter, int skip, int limit) {
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

std::vector<json_value> mongoose::find_all(mongocxx::collection& collection, int skip, int limit){
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

bool mongoose::update_id(mongocxx::collection& collection, const std::string& id, const json_value& update_data, bool upsert) {
    bsoncxx::oid oid{id};
    auto filter = document{} << "_id" << oid << finalize;
    auto update_doc = document{}  << "$set" << to_bson(update_data) << finalize;
    auto options = mongocxx::options::update().upsert(upsert);
    auto result = collection.update_one(filter.view(), update_doc.view(),options);
    return result && (result->modified_count() > 0 || result->upserted_id());
}

bool mongoose::remove_id(mongocxx::collection& collection, const std::string& id) {
    bsoncxx::oid oid{id};
    auto filter = document{} << "_id" << oid << finalize;
    auto result = collection.delete_one(filter.view());
    return result && result->deleted_count() > 0;
}

std::optional<json_value> mongoose::find_by_id(mongocxx::collection& collection, const std::string& id) {
    bsoncxx::oid oid{id};
    auto filter = document{} << "_id" << oid << finalize;
    auto result = collection.find_one(filter.view());
    return result ? std::make_optional(to_json(*result)) : std::nullopt;
}
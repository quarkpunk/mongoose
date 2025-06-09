#ifndef QUARKPUNK_MONGODB_HPP
#define QUARKPUNK_MONGODB_HPP

#include<mongocxx/client.hpp>
#include<mongocxx/pool.hpp>
#include<string>

namespace mongoose{
class mongodb{
public:
    mongodb(const std::string& uri) : pool{mongocxx::uri{uri}}{ ping(); }
    ~mongodb();
    mongodb(const mongodb&) = delete;
    mongodb& operator=(const mongodb&) = delete;
    mongocxx::pool pool;
    bsoncxx::v_noabi::document::value create_objectid(std::string id);
    bsoncxx::v_noabi::document::value create_document(const std::string& oid, const std::string& json);
private:
    void ping();
};
}

#endif
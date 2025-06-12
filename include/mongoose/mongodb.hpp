#ifndef QUARKPUNK_MONGODB_HPP
#define QUARKPUNK_MONGODB_HPP

#include<mongocxx/client.hpp>
#include<mongocxx/pool.hpp>
#include<string>
#include<optional>

namespace mongoose{

struct index_value {
    std::string name;
    std::vector<std::pair<std::string, int>> fields;
    std::optional<bool> unique;
    std::optional<bool> sparse;
    std::optional<int32_t> expire_after_seconds;
    std::optional<std::string> default_language;
};

class mongodb{
public:
    mongodb(const std::string& uri) : pool{mongocxx::uri{uri}}{ ping(); }
    ~mongodb();
    mongodb(const mongodb&) = delete;
    mongodb& operator=(const mongodb&) = delete;
    bool index_create(const std::string& database, const std::string& collection, const index_value& index_value);
    bool index_drop(const std::string& database, const std::string& collection, const std::string& index_name);
    std::vector<std::string> index_list(const std::string& database, const std::string& collection);
    void index_list_print(const std::string& database, const std::string& collection);
public:
    mongocxx::pool pool;
private:
    void ping();
};

}

#endif
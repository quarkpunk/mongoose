#ifndef QUARKPUNK_MONGODB_HPP
#define QUARKPUNK_MONGODB_HPP

#include<mongocxx/client.hpp>
#include<mongocxx/pool.hpp>

namespace mongoose{

class mongodb{
public:
    mongodb(const std::string& uri) : pool{mongocxx::uri{uri}}{ ping(); }
    ~mongodb();
    mongodb(const mongodb&) = delete;
    mongodb& operator=(const mongodb&) = delete;
public:
    mongocxx::pool pool;
private:
    void ping();
};

}

#endif
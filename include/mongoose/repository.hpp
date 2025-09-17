#ifndef QUARKPUNK_MONGOOSE_REPOSITOY_HPP
#define QUARKPUNK_MONGOOSE_REPOSITOY_HPP

#include<mongoose/mongodb.hpp>
#include<mongoose/types/oid.hpp>
#include<mongoose/json.hpp>
#include<bsoncxx/builder/basic/kvp.hpp>
#include<bsoncxx/builder/stream/document.hpp>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;

namespace mongoose{

class repository {
public:
    repository(mongoose::mongodb& mongo,
        const std::string& database,
        const std::string& collection)
        : mongodb(mongo), database(database), collection(collection){}
    virtual ~repository() = default;
public:
    const std::string database;
    const std::string collection;
protected:
    mongoose::mongodb& mongodb;
};

}

#endif
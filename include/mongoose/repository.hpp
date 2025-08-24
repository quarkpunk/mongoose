#ifndef QUARKPUNK_MONGOOSE_REPOSITOY_HPP
#define QUARKPUNK_MONGOOSE_REPOSITOY_HPP

#include<mongoose/mongodb.hpp>

namespace mongoose{

class repository {
public:
    repository(mongoose::mongodb& mongo,
        const std::string& database,
        const std::string& collection)
        : mongodb(mongo), database(database), collection(collection){}
    virtual ~repository() = default;
protected:
    mongoose::mongodb& mongodb;
    const std::string database;
    const std::string collection;
};

}

#endif
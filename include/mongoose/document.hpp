#ifndef QUARKPUNK_MONGOOSE_DOCUMENT_HPP
#define QUARKPUNK_MONGOOSE_DOCUMENT_HPP

#include<bsoncxx/builder/stream/document.hpp>
#include<bsoncxx/json.hpp>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::finalize;

#endif
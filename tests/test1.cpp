#include<mongoose/json.hpp>
#include<mongoose/types/date.hpp>
#include<mongoose/types/oid.hpp>
#include<mongoose/types/location_point.hpp>

static void mongoose_type_date(){
    puts("🏷 test mongoose::type::date type");
    mongoose::type::date date;
    puts("input date: now");
    printf(" ├─ date now (string): %s\n", date.to_string().c_str());
    printf(" └─ date now (timestamp): %li\n", date.to_timestamp());
    puts("input string: 2025-09-10T10:05:45.356Z");
    mongoose::type::date date_parse = mongoose::type::date::from_string("2025-09-10T10:05:45.356Z");
    printf(" ├─ date parse (string): %s\n", date_parse.to_string().c_str());
    printf(" └─ date parse (timestamp): %li\n\n", date_parse.to_timestamp());
}

static void mongoose_type_oid(){
    puts("🏷 test mongoose::type::oid type");
    puts("input string: 68b764430d16fadc6a565075");
    mongoose::type::oid oid("68b764430d16fadc6a565075");
    printf(" ├─ oid export (string): %s\n", oid.to_string().c_str());
    printf(" ├─ oid valid (bool): %s\n", oid.is_valid() == true ? "true" : "false");
    printf(" └─ oid emtpy (bool): %s\n\n", oid.empty() == true ? "true" : "false");
}

static void mongoose_type_geo(){
    puts("🏷 test mongoose::type::location_point type");
    puts("input location: -73.985417 | 40.748805");
    mongoose::type::location_point location;
    location.longitude = -73.985417;
    location.latitude = 40.748805;
    printf(" ├─ location (long|lati): %f/%f\n", location.longitude, location.latitude);
    printf(" └─ location (json/string): %s\n", mongoose::json::to_string(location).c_str());
}

int main(int argc, char const *argv[]){
    mongoose_type_date();
    mongoose_type_oid();
    mongoose_type_geo();
    return 0;
}
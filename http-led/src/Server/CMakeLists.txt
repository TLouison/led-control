add_library(Server)

find_package(httplib CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

target_link_libraries(Server PRIVATE httplib::httplib)
target_link_libraries(Server PRIVATE nlohmann_json::nlohmann_json)

target_sources(Server
    PRIVATE
        Server.cpp

    PUBLIC
        FILE_SET HEADERS
        FILES
            Server.h
)

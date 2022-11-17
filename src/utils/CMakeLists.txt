add_library(utils STATIC
    logs.cpp
    logs.hh
)

target_include_directories(utils PUBLIC
    .
    ../../includes
)
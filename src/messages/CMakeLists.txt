set(CMD_sources
    CMD/load.cpp
    CMD/load.hh
    CMD/loadResponse.cpp
    CMD/loadResponse.hh
)

set(RPC_sources
    RPC/appendEntries.cpp
    RPC/appendEntries.hh
    RPC/requestVote.cpp
    RPC/requestVote.hh
    RPC/vote.cpp
    RPC/vote.hh
)

add_library(messages STATIC
    "message.cpp"
    "message.hh"
    mpi_wrappers.cpp
    mpi_wrappers.hh
    responseToClient.cpp
    responseToClient.hh
    status.cpp
    status.hh
    ${CMD_sources}
    ${RPC_sources}
)

target_include_directories(messages PUBLIC
    .
    ../../includes
)
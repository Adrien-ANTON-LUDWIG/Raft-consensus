set(CMD_sources
    CMD/append.cpp
    CMD/append.hh
    CMD/appendResponse.cpp
    CMD/appendResponse.hh
    CMD/delete.cpp
    CMD/delete.hh
    CMD/deleteResponse.cpp
    CMD/deleteResponse.hh
    CMD/list.cpp
    CMD/list.hh
    CMD/listResponse.cpp
    CMD/listResponse.hh
    CMD/load.cpp
    CMD/load.hh
    CMD/loadResponse.cpp
    CMD/loadResponse.hh
)

set(RPC_sources
    RPC/appendEntries.cpp
    RPC/appendEntries.hh
    RPC/appendEntriesResponse.cpp
    RPC/appendEntriesResponse.hh
    RPC/requestVote.cpp
    RPC/requestVote.hh
    RPC/vote.cpp
    RPC/vote.hh
)

set (REPL_sources
    REPL/crash.cpp
    REPL/crash.hh
    REPL/info.cpp
    REPL/info.hh
    REPL/speed.cpp
    REPL/speed.hh
    REPL/start.cpp
    REPL/start.hh
    REPL/stop.hh
    REPL/stop.cpp
)

add_library(messages STATIC
    "message.cpp"
    "message.hh"
    mpi_wrappers.cpp
    mpi_wrappers.hh
    responseToClient.cpp
    responseToClient.hh
    redirect.cpp
    redirect.hh
    ${CMD_sources}
    ${RPC_sources}
    ${REPL_sources}
)

target_include_directories(messages PUBLIC
    .
    ../../includes
)
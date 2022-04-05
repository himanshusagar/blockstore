# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

project(blockstore)

file(GLOB SRCS_Server server.cc server_rpc.h server_rpc.cpp client_rpc.h client_rpc.cpp)
file(GLOB SRCS_Client client.cc client_rpc.h client_rpc.cpp)
file(GLOB SRCS_Measure_Client measure_client.cc client_rpc.h client_rpc.cpp)


add_executable(server ${SRCS_Server})
add_executable(dumbClient ${SRCS_Client})
add_executable(client ${SRCS_Measure_Client})

target_link_libraries(server
        Threads::Threads
        gRPC::grpc++
        p3protolib)
add_dependencies(server p3protolib)
set_target_properties(server PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

target_link_libraries(client
        Threads::Threads
        gRPC::grpc++
        p3protolib)
add_dependencies(client p3protolib)
set_target_properties(client PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

target_link_libraries(dumbClient
        Threads::Threads
        gRPC::grpc++
        p3protolib)
add_dependencies(dumbClient p3protolib)
set_target_properties(dumbClient PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)


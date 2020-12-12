# Include generated *.pb.h files
include_directories("${service_proto_include_dir}")

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

file(GLOB SRCS "*.cpp")

add_executable(drc-proxy
    ${SRCS}
    ${service_proto_srcs}
    ${service_grpc_srcs})
add_dependencies(drc-proxy generate-proto)
target_link_libraries(drc-proxy
    protobuf::libprotobuf
    grpc++_reflection
    gRPC::grpc++)

set_source_files_properties(
    "${service_proto_srcs}"
    "${service_grpc_srcs}"
    PROPERTIES GENERATED TRUE
)


find_program(_PROTOBUF_PROTOC protoc)
find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)

# Proto file
get_filename_component(service_proto "service.proto" ABSOLUTE)
get_filename_component(service_proto_path "${service_proto}" PATH)

# Generated sources
set(service_proto_srcs_internal "${CMAKE_CURRENT_BINARY_DIR}/service.pb.cc")
set(service_proto_hdrs_internal "${CMAKE_CURRENT_BINARY_DIR}/service.pb.h")
set(service_grpc_srcs_internal "${CMAKE_CURRENT_BINARY_DIR}/service.grpc.pb.cc")
set(service_grpc_hdrs_internal "${CMAKE_CURRENT_BINARY_DIR}/service.grpc.pb.h")

add_custom_command(
      OUTPUT
        "${service_proto_srcs_internal}"
        "${service_proto_hdrs_internal}"
        "${service_grpc_srcs_internal}"
        "${service_grpc_hdrs_internal}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${service_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${service_proto}"
      DEPENDS "${service_proto}")

add_custom_target(generate-proto DEPENDS
    "${service_proto_srcs_internal}"
    "${service_proto_hdrs_internal}"
    "${service_grpc_srcs_internal}"
    "${service_grpc_hdrs_internal}")

set(service_proto_include_dir "${CMAKE_CURRENT_BINARY_DIR}" PARENT_SCOPE)
set(service_proto_srcs "${service_proto_srcs_internal}" PARENT_SCOPE)
set(service_proto_hdrs "${service_proto_hdrs_internal}" PARENT_SCOPE)
set(service_grpc_srcs "${service_grpc_srcs_internal}" PARENT_SCOPE)
set(service_grpc_hdrs "${service_grpc_hdrs_internal}" PARENT_SCOPE)

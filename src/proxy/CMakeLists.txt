pkg_check_modules(LIBDRC QUIET IMPORTED_TARGET libdrc)

if (LIBDRC_FOUND)
  message(STATUS "Found libdrc ${LIBDRC_VERSION}")
  
  pkg_check_modules(LIBSWSCALE REQUIRED IMPORTED_TARGET libswscale)
  pkg_check_modules(X264 REQUIRED IMPORTED_TARGET x264)
  message(STATUS "Using libswscale ${LIBSWSCALE_VERSION}")
  message(STATUS "Using x264 ${X264}")

  # Include generated *.pb.h files
  include_directories("${service_proto_include_dir}")

  file(GLOB SRCS "*.cpp")

  add_executable(drc-proxy
      ${SRCS}
      ${service_proto_srcs}
      ${service_grpc_srcs})
  add_dependencies(drc-proxy generate-proto)
  target_link_libraries(drc-proxy
      protobuf::libprotobuf
      grpc++_reflection
      gRPC::grpc++
      PkgConfig::LIBDRC
      PkgConfig::X264
      PkgConfig::LIBSWSCALE)

  set_source_files_properties(
      "${service_proto_srcs}"
      "${service_grpc_srcs}"
      PROPERTIES GENERATED TRUE
  )
endif()

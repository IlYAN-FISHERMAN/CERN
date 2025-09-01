find_package(GTest REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(absl REQUIRED)

# set(PROTO_FILES ../proto/IoBuffer.proto)
#
# protobuf_generate( TARGET proto_objs # cible qui regroupe les fichiers générés
# LANGUAGE cpp # on veut du C++ PROTOS ${PROTO_FILES} # liste de .proto
# IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR} # où chercher les imports
# PROTOC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR} # fichiers générés ici )

add_library(io_monitor STATIC IoStat.cc IoMap.cc IoAggregate.cc
                              IoAggregateMap.cc ../proto/IoBuffer.pb.cc)

target_include_directories(io_monitor PUBLIC ${CMAKE_SOURCE_DIR}/include
                                             /opt/eos/grpc/include)

target_link_directories(io_monitor PUBLIC /opt/eos/grpc/lib64)

target_link_libraries(
  io_monitor PUBLIC protobuf pthread absl_log_internal_check_op absl_base
                    absl_log_internal_message)

target_include_directories(io_monitor PUBLIC ${CMAKE_SOURCE_DIR}/include)

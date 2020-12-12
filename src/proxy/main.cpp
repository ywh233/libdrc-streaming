#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include "service.pb.h"
#include "service.grpc.pb.h"

namespace drc {

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

class DrcStreamServiceImpl final : public DrcStreamService::Service {
  Status SendH264Chunks(ServerContext* context,
                        const H264ChunksRequest* request,
                        H264ChunksResponse* response) {
    return Status::OK;
  }
};

void RunServer() {
  std::string server_address("0.0.0.0:5179");
  DrcStreamServiceImpl service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

}  // namespace drc

int main(int argc, char const *argv[]) {
  drc::RunServer();

  return 0;
}

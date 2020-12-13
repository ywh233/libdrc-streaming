#include <drc-streaming/internal/video-converter.h>
#include <drc-streaming/internal/h264-encoder.h>
#include <grpcpp/grpcpp.h>
#include <unistd.h>

#include <iostream>
#include <tuple>
#include <vector>

#include "service.pb.h"
#include "service.grpc.pb.h"

namespace drc {

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class DrcStreamClient {
 public:
  DrcStreamClient(std::shared_ptr<Channel> channel) :
      stub_(DrcStreamService::NewStub(channel)) {}

  void SendH264Chunks(const H264ChunkArray& chunk_array, bool is_idr) {
    ClientContext context;

    H264ChunksRequest request;
    for (const auto& chunk_tuple : chunk_array) {
      const byte* bytes;
      size_t size;
      std::tie(bytes, size) = chunk_tuple;
      request.add_chunks(bytes, size);
      request.set_is_idr(is_idr);
    }
 
    H264ChunksResponse response;
    Status status = stub_->SendH264Chunks(&context, request, &response);
    if (!status.ok()) {
      std::cerr << "RPC failed: " << status.error_code() << ", "
                << status.error_message() << std::endl;
    }
  }

 private:
  std::unique_ptr<DrcStreamService::Stub> stub_;
};

}  // namespace drc

namespace {

enum ColorComponent {
  kCCBlue = 0,
  kCCGreen = 1,
  kCCRed = 2,
  kCCAlpha = 3,
};

uint8_t* ColorAsU8A(uint32_t* color_ptr) {
  return reinterpret_cast<uint8_t*>(color_ptr);
}

uint32_t ColorOf(uint8_t blue, uint8_t green, uint8_t red, uint8_t alpha = 0) {
  uint32_t val;
  uint8_t* val_u8a = ColorAsU8A(&val);
  val_u8a[kCCBlue] = blue;
  val_u8a[kCCGreen] = green;
  val_u8a[kCCRed] = red;
  val_u8a[kCCAlpha] = alpha;
  return val;
}

// Returns true if |current_color| is changed.
bool MoveTowards(uint32_t target_color, uint32_t* current_color) {
  uint8_t* target_color_u8a = ColorAsU8A(&target_color);
  uint8_t* current_color_u8a = ColorAsU8A(current_color);
  bool component_changed = false;

  for (int i = 0; i < 4; i++) {
    int16_t diff =
        static_cast<int16_t>(target_color_u8a[i]) - current_color_u8a[i];
    if (diff == 0) {
      continue;
    }
    if (diff > 0) {
      current_color_u8a[i] += 3;
      if (current_color_u8a[i] > target_color_u8a[i]) {
        current_color_u8a[i] = target_color_u8a[i];
      }
    } else {
      current_color_u8a[i] -= 3;
      if (current_color_u8a[i] < target_color_u8a[i]) {
        current_color_u8a[i] = target_color_u8a[i];
      }
    }
    component_changed = true;
  }

  return component_changed;
}

void FillColor(std::vector<uint32_t>* frame, uint32_t color) {
  for (size_t i = 0; i < frame->size(); i++) {
    frame->data()[i] = color;
  }
}

}  // namespace

int main(int argc, char const *argv[]) {
  std::string target_str = "192.168.19.3:5179";
  drc::DrcStreamClient stream_client(
    grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));

  drc::VideoConverter video_converter;
  drc::H264Encoder h264_encoder;

  uint32_t target_colors[8] = {
    ColorOf(255, 0, 0),
    ColorOf(0, 255, 0),
    ColorOf(255, 255, 0),
    ColorOf(0, 0, 255),
    ColorOf(255, 0, 255),
    ColorOf(0, 255, 255),
    ColorOf(255, 255, 255),
    ColorOf(0, 0, 0),
  };
  size_t current_target = 1u;
  uint32_t current_color = target_colors[0];

  std::vector<uint32_t> frame;
  frame.reserve(864 * 480);

  for (size_t i = 0u; i < 864 * 480; i++) {
    frame.push_back(current_color);
  }

  while (true) {
    if (!MoveTowards(target_colors[current_target], &current_color)) {
      current_target = (current_target + 1) % 8;
      continue;
    }

    FillColor(&frame, current_color);

    drc::u8* frame_data_begin = ColorAsU8A(frame.data());
    std::vector<drc::u8> frame_out(
        frame_data_begin, 
        frame_data_begin + frame.size() * 4);
    auto yuv_frame = video_converter.ConvertFrame(
        &frame_out, std::make_tuple(864, 480, drc::PixelFormat::kBGRA, false,
                                    false, false));
    auto chunks = h264_encoder.Encode(yuv_frame, true);
    stream_client.SendH264Chunks(chunks, true);

    usleep(16683);  // 59.94Hz
  }

  return 0;
}

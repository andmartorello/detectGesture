#include <vector>

#include "libs/base/filesystem.h"
#include "libs/base/led.h"
#include "libs/camera/camera.h"
#include "libs/tensorflow/detection.h"
#include "libs/tensorflow/utils.h"
#include "libs/tpu/edgetpu_manager.h"
#include "libs/tpu/edgetpu_op.h"
#include "third_party/tflite-micro/tensorflow/lite/micro/micro_error_reporter.h"
#include "third_party/tflite-micro/tensorflow/lite/micro/micro_interpreter.h"
#include "third_party/tflite-micro/tensorflow/lite/micro/micro_mutable_op_resolver.h"

namespace coralmicro {

namespace {

constexpr char kModelPath[] = "/models/model_edgetpu.tflite";
constexpr int kTensorArenaSize = 8 * 1024 * 1024;
STATIC_TENSOR_ARENA_IN_SDRAM(tensor_arena, kTensorArenaSize);

[[noreturn]] void Main() {
  LedSet(Led::kStatus, true);

  std::vector<uint8_t> model;
  LfsReadFile(kModelPath, &model);

  auto tpu_context = EdgeTpuManager::GetSingleton()->OpenDevice();

  tflite::MicroErrorReporter error_reporter;
  tflite::MicroMutableOpResolver<3> resolver;
  resolver.AddDequantize();
  resolver.AddDetectionPostprocess();
  resolver.AddCustom(kCustomOp, RegisterCustomOp());

  tflite::MicroInterpreter interpreter(tflite::GetModel(model.data()), resolver,
                                       tensor_arena, kTensorArenaSize, &error_reporter);

  interpreter.AllocateTensors();

  CameraTask::GetSingleton()->SetPower(true);
  CameraTask::GetSingleton()->Enable(CameraMode::kStreaming);

  auto* input_tensor = interpreter.input_tensor(0);
  int model_height = input_tensor->dims->data[1];
  int model_width = input_tensor->dims->data[2];

  while (true) {
    CameraFrameFormat fmt{CameraFormat::kRgb, CameraFilterMethod::kBilinear, 
                          CameraRotation::k270, model_width, model_height, false,
                          tflite::GetTensorData<uint8_t>(input_tensor)};

    CameraTask::GetSingleton()->GetFrame({fmt});

    interpreter.Invoke();

    TfLiteTensor* output_tensor = interpreter.output_tensor(0);
    uint8_t* output_data = tflite::GetTensorData<uint8_t>(output_tensor);
    
    int max_index = 0;
    float max_value = (output_data[0] / 255.0) * 100.0; // assuming the data is uint8_t and needs scaling
    for (int i = 1; i < 3; ++i) {
      float curr_value = (output_data[i] / 255.0) * 100.0;
      if (curr_value > max_value) {
        max_value = curr_value;
        max_index = i;
      }
    }

    const char* class_names[] = {"ok", "palm", "peace"};
    // Controllo se la massima confidenza è maggiore del 95%
	  if (max_value > 95.0) {
	    // Stampa solo se la confidenza è maggiore del 95%
	    printf("Highest confidence: %s (%f%%)\r\n", class_names[max_index], max_value);
	  }
  }
}

}  // namespace
} //coralmicro
extern "C" void app_main(void* param) {
  (void)param;
  coralmicro::Main();
}



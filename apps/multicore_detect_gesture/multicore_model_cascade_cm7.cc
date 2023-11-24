// Copyright 2022 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "libs/base/filesystem.h"
#include "libs/base/http_server_handlers.h"
#include "libs/base/ipc_m7.h"
#include "libs/base/led.h"
#include "libs/base/mutex.h"
#include "libs/base/network.h"
#include "libs/base/reset.h"
#include "libs/base/strings.h"
#include "libs/base/watchdog.h"
#include "libs/camera/camera.h"
#include "libs/libjpeg/jpeg.h"
#include "libs/rpc/rpc_http_server.h"
#include "libs/tensorflow/posenet.h"
#include "libs/tpu/edgetpu_manager.h"
#include "third_party/freertos_kernel/include/FreeRTOS.h"
#include "third_party/freertos_kernel/include/semphr.h"
#include "third_party/freertos_kernel/include/task.h"
#include "third_party/mjson/src/mjson.h"
#include "third_party/tflite-micro/tensorflow/lite/micro/micro_interpreter.h"


#include <cstdio>
#include <cstring>
#include <memory>
#include <utility>
#include <vector>

#include "libs/tensorflow/detection.h"
#include "libs/tensorflow/utils.h"
#include "libs/tpu/edgetpu_op.h"
#include "third_party/tflite-micro/tensorflow/lite/micro/micro_error_reporter.h"
#include "third_party/tflite-micro/tensorflow/lite/micro/micro_mutable_op_resolver.h"
int consecutiveLowConfidence = 0;
namespace coralmicro {
namespace {
constexpr char kModelPath[] = "/models/model_edgetpu.tflite";

constexpr int kTensorArenaSize = 8 * 1024 * 1024; // 8MB

STATIC_TENSOR_ARENA_IN_SDRAM(tensor_arena, kTensorArenaSize);

[[noreturn]] void Main() {
  printf("Multicore Model Cascade!\r\n");

  constexpr WatchdogConfig wdt_config = {
      .timeout_s = 8,
      .pet_rate_s = 3,
      .enable_irq = false,
  };
  WatchdogStart(wdt_config);

  auto tpu_context =
      EdgeTpuManager::GetSingleton()->OpenDevice(PerformanceMode::kMax);
  if (!tpu_context) {
    printf("Failed to get tpu context.\r\n");
    vTaskSuspend(nullptr);
  }
  std::vector<uint8_t> model;
  if (!LfsReadFile(kModelPath, &model)) {
    printf("ERROR: Failed to read model: %s\r\n", kModelPath);
    vTaskSuspend(nullptr);
  }

  tflite::MicroErrorReporter error_reporter;
  tflite::MicroMutableOpResolver<3> resolver;
  resolver.AddDequantize(); 
  resolver.AddDetectionPostprocess(); 
  resolver.AddCustom(kCustomOp, RegisterCustomOp());  


  tflite::MicroInterpreter interpreter(tflite::GetModel(model.data()), resolver, 
                                       tensor_arena, kTensorArenaSize, &error_reporter);

      
  if (interpreter.AllocateTensors() != kTfLiteOk) {
    printf("Failed to allocate tensor\r\n");
    vTaskSuspend(nullptr);
  }

  auto* input_tensor = interpreter.input_tensor(0);
  int model_height = input_tensor->dims->data[1];
  int model_width = input_tensor->dims->data[2];

  IpcM7::GetSingleton()->RegisterAppMessageHandler(
      [handle = xTaskGetCurrentTaskHandle()](const uint8_t[]) {
        printf("Message received\r\n");
        vTaskResume(handle);
      });

  IpcM7::GetSingleton()->StartM4();

  vTaskSuspend(nullptr);

  while (true) {

    consecutiveLowConfidence = 0;
    CameraFrameFormat fmt{CameraFormat::kRgb, CameraFilterMethod::kBilinear, 
                          CameraRotation::k270, model_width, model_height, false,
                          tflite::GetTensorData<uint8_t>(input_tensor)};
    CameraTask::GetSingleton()->Enable(CameraMode::kStreaming);

    while (true) {
      
      CameraTask::GetSingleton()->GetFrame({fmt});

      TfLiteStatus invoke_status = interpreter.Invoke();
      if (invoke_status != kTfLiteOk) {
          printf("ERROR: Model invocation failed with status %d\r\n", invoke_status);
      }

      TfLiteTensor* output_tensor = interpreter.output_tensor(0);
      uint8_t* output_data = tflite::GetTensorData<uint8_t>(output_tensor);

      int max_index = 0;
      float max_value = (output_data[0] / 255.0) * 100.0; 
      for (int i = 1; i < 3; ++i) {
        float curr_value = (output_data[i] / 255.0) * 100.0;
        if (curr_value > max_value) {
          max_value = curr_value;
          max_index = i;
        }
      }

      const char* class_names[] = {"ok", "palm", "peace"};

      if (max_value > 95.0) {
        printf("Highest confidence: %s (%f%%)\r\n", class_names[max_index], max_value);
        vTaskDelay(pdMS_TO_TICKS(1000));
        if (consecutiveLowConfidence != 0) {
          consecutiveLowConfidence = 0;
        }
      } else {
        consecutiveLowConfidence++; 
        if (consecutiveLowConfidence >= 5) {
          break; 
        }
      }

    }

    printf("M7 stopped\r\n");
    CameraTask::GetSingleton()->Disable();
    IpcMessage msg{};
    msg.type = IpcMessageType::kApp;
    IpcM7::GetSingleton()->SendMessage(msg);
    vTaskSuspend(nullptr);
  }
}
}  
}  

extern "C" void app_main(void* param) {
  (void)param;
  coralmicro::Main();
}
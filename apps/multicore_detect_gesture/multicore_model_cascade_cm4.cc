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

#include <cstdio>


#include "libs/base/filesystem.h"
#include "libs/base/ipc_m4.h"
#include "libs/base/led.h"
#include "libs/base/main_freertos_m4.h"
#include "libs/camera/camera.h"
#include "libs/tensorflow/utils.h"
#include "third_party/freertos_kernel/include/FreeRTOS.h"
#include "third_party/freertos_kernel/include/task.h"
#include "third_party/freertos_kernel/include/timers.h"
#include "third_party/tflite-micro/tensorflow/lite/micro/micro_error_reporter.h"
#include "third_party/tflite-micro/tensorflow/lite/micro/micro_interpreter.h"
#include "third_party/tflite-micro/tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "third_party/tflite-micro/tensorflow/lite/schema/schema_generated.h"
#include "semphr.h"

bool isReadyForSuspension = false;

namespace coralmicro {


[[noreturn]] void Main() {
  // This handler resume this m4 task, as soon as signal from m7 is received.
  IpcM4::GetSingleton()->RegisterAppMessageHandler(
      [handle = xTaskGetCurrentTaskHandle()](const uint8_t[]) {
        vTaskResume(handle);
      });
  CameraTask::GetSingleton()->Init(I2C5Handle());
  CameraTask::GetSingleton()->SetPower(false);
  vTaskDelay(pdMS_TO_TICKS(100));
  CameraTask::GetSingleton()->SetPower(true);
  CameraMotionDetectionConfig config{};
  LedSet(Led::kStatus, true);


  while (true) {
    printf("M4 Started, detecting person...\r\n");
    //
    isReadyForSuspension = false;
    //
    vTaskDelay(pdMS_TO_TICKS(100));
    //
    CameraTask::GetSingleton()->GetMotionDetectionConfigDefault(config);
      config.cb = [](void* /*param*/) {
        if(isReadyForSuspension == false){
          printf("Motion detected\r\n");
          //LedSet(Led::kUser, true);
          isReadyForSuspension = true;          
        }
      };

    CameraTask::GetSingleton()->SetMotionDetectionConfig(config);
    CameraTask::GetSingleton()->Enable(CameraMode::kStreaming);

    while (true) {
      vTaskDelay(pdMS_TO_TICKS(100));
      printf("Inizio secondo ciclo M4\r\n");
      if (isReadyForSuspension) {
          //LedSet(Led::kUser, false);
          CameraTask::GetSingleton()->Disable();
          printf("M4 stopped\r\n");
          //isReadyForSuspension = false;
          break; 
      }
    }
    IpcMessage msg{};
    msg.type = IpcMessageType::kApp;
    IpcM4::GetSingleton()->SendMessage(msg);
    vTaskDelay(pdMS_TO_TICKS(100));
    vTaskSuspend(nullptr);

  }
}
}  // namespace coralmicro

extern "C" void app_main(void* param) {
  (void)param;
  coralmicro::Main();
}
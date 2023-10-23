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

// Inclusione delle librerie necessarie per il funzionamento del programma.
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

// Percorso al modello di machine learning che sarà utilizzato per l'inferenza.
constexpr char kModelPath[] = "/models/model_edgetpu.tflite";

// Definizione della dimensione dell'arena per i tensori (memoria allocata per l'esecuzione del modello).
constexpr int kTensorArenaSize = 8 * 1024 * 1024; // 8MB

// Allocazione dello spazio per l'arena del tensore nell'SDRAM (memoria statica).
STATIC_TENSOR_ARENA_IN_SDRAM(tensor_arena, kTensorArenaSize);

// Funzione principale del programma.
[[noreturn]] void Main() {
  // Accende il LED di stato per indicare l'inizio del processo.
  LedSet(Led::kStatus, true);

  // Carica il modello di machine learning dal file system nella memoria.
  std::vector<uint8_t> model;
  LfsReadFile(kModelPath, &model);

  // Inizializza il contesto del dispositivo Edge TPU (acceleratore hardware per il machine learning).
  auto tpu_context = EdgeTpuManager::GetSingleton()->OpenDevice();

  // Crea un reporter degli errori per TensorFlow Lite.
  tflite::MicroErrorReporter error_reporter;

  // Prepara il risolutore di operazioni con le operazioni necessarie per il modello da utilizzare.
  tflite::MicroMutableOpResolver<3> resolver;
  resolver.AddDequantize();  // Per operazioni di dequantizzazione.
  resolver.AddDetectionPostprocess();  // Per il post-processing del rilevamento.
  resolver.AddCustom(kCustomOp, RegisterCustomOp());  // Aggiunge operazioni personalizzate.

  // Crea un interprete TensorFlow Lite con il modello caricato, il risolutore, e l'arena del tensore.
  tflite::MicroInterpreter interpreter(tflite::GetModel(model.data()), resolver, 
                                       tensor_arena, kTensorArenaSize, &error_reporter);

  // Alloca i tensori necessari per l'interprete.
  interpreter.AllocateTensors();

  // Accende la fotocamera e imposta la modalità di streaming.
  CameraTask::GetSingleton()->SetPower(true);
  CameraTask::GetSingleton()->Enable(CameraMode::kStreaming);

  // Ottiene il tensore di input e le sue dimensioni (larghezza e altezza) per il modello.
  auto* input_tensor = interpreter.input_tensor(0);
  int model_height = input_tensor->dims->data[1];
  int model_width = input_tensor->dims->data[2];

  // Ciclo principale di elaborazione delle immagini e inferenza.
  while (true) {
    // Configura il formato del frame della fotocamera per adattarlo all'input del modello.
    CameraFrameFormat fmt{CameraFormat::kRgb, CameraFilterMethod::kBilinear, 
                          CameraRotation::k270, model_width, model_height, false,
                          tflite::GetTensorData<uint8_t>(input_tensor)};

    // Cattura il frame corrente dalla fotocamera.
    CameraTask::GetSingleton()->GetFrame({fmt});

    // Esegue l'inferenza sul frame catturato.
    interpreter.Invoke();

    // Recupera il tensore di output dall'interprete.
    TfLiteTensor* output_tensor = interpreter.output_tensor(0);
    uint8_t* output_data = tflite::GetTensorData<uint8_t>(output_tensor);

    // Analizza i dati di output per determinare il risultato con la maggiore probabilità.
    int max_index = 0;
    float max_value = (output_data[0] / 255.0) * 100.0; // Scala i dati da uint8_t.
    for (int i = 1; i < 3; ++i) {
      float curr_value = (output_data[i] / 255.0) * 100.0;
      if (curr_value > max_value) {
        max_value = curr_value;
        max_index = i;
      }
    }

    // Nomi delle classi per l'output dell'inferenza.
    const char* class_names[] = {"ok", "palm", "peace"};

    // Se la confidenza del risultato è maggiore del 95%, stampa il risultato.
    if (max_value > 95.0) {
      printf("Highest confidence: %s (%f%%)\r\n", class_names[max_index], max_value);
    }
  }
}

}  // namespace anonimo
}  // namespace coralmicro

// Funzione di ingresso principale, invocata all'avvio dell'applicazione.
extern "C" void app_main(void* param) {
  (void)param; // Ignora il parametro non utilizzato.
  coralmicro::Main(); // Avvia la funzione principale del programma.
}



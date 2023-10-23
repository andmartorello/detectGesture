# Gesture Detection with Coral Dev Board Micro

This repository harnesses the power of the Coral Dev Board Micro to bring real-time, efficient, and low-power AI inferences to the edge, specifically focusing on gesture recognition through an optimized and quantized AI model.

## Table of Contents
- [Overview: Pioneering Edge AI Technology](#overview-pioneering-edge-ai-technology)
- [Quantized AI Model: Efficiency and Power](#quantized-ai-model-efficiency-and-power)
- [Maximizing Potential on Compact Devices](#maximizing-potential-on-compact-devices)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation and Execution](#installation-and-execution)
- [Visualizing Serial Outputs](#visualizing-serial-outputs)
- [Contribute](#contribute)
- [Support and Community](#support-and-community)

## Overview: Pioneering Edge AI Technology

In artificial intelligence, edge devices' capability to perform intricate processing is transforming various industries. The Coral Dev Board Micro, through its efficient, real-time AI inferences with minimal power usage, sits at the cutting edge of this transformation.

## Quantized AI Model: Efficiency and Power

The essence of our approach is the quantized AI model, which reduces the numerical precision of the model's weights but maintains high inference accuracy. The advantages include:

- **Computational Efficiency**: Less computational operations are needed, perfect for devices with power constraints.
- **Model Size Reduction**: Smaller models mean faster deployment and efficient memory use.
- **Real-time Execution**: Ideal for immediate decision-making applications, like gesture recognition.

This on-device processing capability ensures unparalleled interactivity where immediate response is crucial, maintaining high privacy standards.

## Maximizing Potential on Compact Devices

Our implementation harnesses one of the market's most efficient chips, avoiding the bulkiness and high costs of traditional deep learning solutions. This fusion of miniaturization and artificial intelligence paves the way for innovative applications on embedded systems.

## Getting Started

### Prerequisites

- Software: Ensure `git`, `bash`, and `python3` are installed on your system.
- Hardware: A connected Coral Dev Board Micro.

### Installation and Execution

Follow these steps for setting up and running the gesture recognition system:

1. **Clone the Repository**:
    ```sh
    git clone --recurse-submodules -j8 https://github.com/andmartorello/detectGesture
    ```
2. **Navigate to the Project Directory**:
    ```sh
    cd detectGesture
    ```
3. **Copy Required Files**:
    ```sh
    ./copy_to_submodule.sh
    ```
4. **Set Up the Environment**:
    Navigate to the `coralmicro` directory and set up the environment.
    ```sh
    cd coralmicro
    bash setup.sh
    ```
5. **Flash the Program**:
    Use the `flashtool` script to flash the `detect_gesture` program onto the Coral Dev Board.
    ```sh
    python3 scripts/flashtool.py -e detect_gesture
    ```
6. **(Optional) Custom Modifications**:
    Feel free to modify the `detect_gesture` file to suit your needs. If you make changes, recompile the program.
    ```sh
    make -C build/examples/detect_gesture
    ```
    Flash your custom program using the `flashtool` script.
    ```sh
    python3 scripts/flashtool.py -e detect_gesture --nodata
    ```

## Visualizing Serial Outputs

Monitoring your application's performance in real-time is crucial. Here's how to view serial outputs using `screen` on Unix-based systems:

1. **Connect the Device**: Use a USB cable.
2. **Identify the Serial Port**: Commonly `/dev/ttyACM0` for the Coral Dev Board Micro.
3. **Start the Screen Session**: Replace `/dev/ttyACM0` if necessary.
    ```sh
    screen /dev/ttyACM0 115200
    ```
4. **Interact and Monitor**: View real-time data from your device.
5. **Terminate the Session**: Press `Ctrl-A` and then `:quit`.

> Note: Ensure you have the necessary permissions for serial communication on your system.

## Contribute

Contributions are what make the open-source community an incredible place to learn, inspire, and create. Any contributions to this project are **greatly appreciated**.

- Fork the Project
- Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
- Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
- Push to the Branch (`git push origin feature/AmazingFeature`)
- Open a Pull Request

## Support and Community

For support, feature requests, or inquiries, please refer to the [issues](https://github.com/andmartorello/detectGesture/issues) section of this repository. Feel free to reach out, join discussions, and be a part of our community!

---

## Attribuzione

Questo progetto si basa su ["coralmicro"](https://github.com/google-coral/coralmicro) 

Distribuito sotto licenza Apache 2.0. Per maggiori informazioni, consultare il file LICENSE.

Project Link: [https://github.com/andmartorello/detectGesture](https://github.com/andmartorello/detectGesture)

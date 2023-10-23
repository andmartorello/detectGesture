# Gesture Detection: Revolutionizing Edge AI with Coral Dev Board Micro
This repository features an innovative project that leverages the Coral Dev Board Micro to perform real-time AI inferences for gesture recognition. By implementing an optimized and quantized AI model, this system can operate with minimal latency and extremely low power consumption, making it ideal for resource-constrained devices.

# Pioneering Edge AI Technology
In the realm of artificial intelligence, the ability to conduct complex processing directly on edge devices is revolutionizing myriad industries. The Coral Dev Board Micro stands at the forefront of this revolution, with its capability to execute real-time AI inferences with minimal power consumption and low latency. This feat is made possible through the deployment of optimized, quantized machine learning models tailored for constrained-resource systems.

## Quantized AI Model: Where Efficiency Meets Power
Our approach's hallmark lies in the utilization of a quantized AI model. Quantization involves reducing the numerical precision of the model's weights, a technique that, paradoxically, minimally impacts inference accuracy while bestowing immense benefits:

Computational Efficiency: Quantized models require fewer computational operations, allowing for faster inferences and drastically reduced power consumption, ideal for battery-operated or power-restricted devices.

Model Size Reduction: Through quantization, models become significantly smaller. This facilitates quicker deployment and more efficient memory usage, crucial on devices with limited storage resources.

Real-time Execution: With reduced computational complexity, quantized models can perform inferences in real-time, making them perfect for applications requiring instant decisions, such as gesture recognition or autonomous navigation.

Within the context of gesture recognition, the ability to process data on-device unlocks unprecedented interactivity in scenarios where latency is critical, and bandwidth is scarce. All while upholding high privacy standards, as sensitive data can be processed locally without the need for transmission to a remote server.

## Maximizing Potential on Compact Devices
The Dev Board Micro is a testament to the power of miniaturization combined with artificial intelligence. While some deep learning solutions demand bulky, expensive hardware, our implementation is streamlined for one of the most efficient chips on the market. This approach enables a new range of applications previously unreachable for traditional, resource-intensive AI models on embedded systems.

Through strategic use of quantization and hardware-specific optimization, we're bringing artificial intelligence directly into the real world, overcoming limitations that once held back edge computing. With this technology, we're laying the groundwork for a future where AI is not just accessible but integrable across all facets of our everyday life.


Continue reading to try out the example.


# Documentation

+ [Get Started with the Dev Board Micro](https://coral.ai/docs/dev-board-micro/get-started/)

+ [Get Started with Arduino](https://coral.ai/docs/dev-board-micro/arduino/)

+ [Build an out-of-tree project](https://github.com/google-coral/coralmicro-out-of-tree-sample/blob/main/README.md)

+ [coralmicro API reference](http://coral.ai/docs/reference/micro/)

+ [coralmicro examples](/examples/)

# Getting Started
## Prerequisites
Ensure that you have git, bash, and python3 installed on your system.
A connected and set up Coral Dev Board Micro.

## Installation and Execution Instructions
Follow these steps to set up and use the gesture recognition system:

### Clone the Repository
To get a local copy of the project, open a terminal, and clone this repository using the following command:

```bash
git clone --recurse-submodules -j8 https://github.com/andmartorello/detectGesture
```

### Navigate to the Project Directory

```bash
cd detectGesture
```

### Copy Necessary Files into the Submodule

```bash
./copy_to_submodule.sh
```

### Navigate to the coralmicro Directory and Set Up the Environment

```bash
cd coralmicro
bash setup.sh
```

### Use the Flashtool Script
Execute the flashtool script to flash the detect_gesture program onto your Coral Dev Board:

```bash
python3 scripts/flashtool.py -e detect_gesture
```

### (Optional) Modify the Program
If you wish, you can make changes to the detect_gesture file. After making changes, compile the program using the following command:

```bash
make -C build/examples/detect_gesture
```

### Flash the Modified Program
You can then flash your customized program onto the Coral Dev Board using the flashtool script again:

```bash
python3 scripts/flashtool.py -e detect_gesture --nodata
```

### Testing Gesture Recognition
After flashing the program onto your Coral Dev Board, you should be all set to test the gesture recognition system. Follow the on-screen instructions on your device to interact with the system and see the recognition results.

# Visualizing Serial Outputs
Monitoring the real-time performance of your application on the Coral Dev Board Micro is crucial for debugging and understanding your system's behavior. Serial communication is a window into this world, and here's how you can easily view serial outputs.

## Using screen on Unix-based Systems
The screen command is a powerful utility available on Unix-based systems, allowing you to interact with your device through serial communication. Follow these steps to use screen to view outputs from your Coral Dev Board Micro:

Connect the Device: Ensure your Coral Dev Board Micro is connected to your computer via a USB cable.

Identify the Serial Port: Before you can start the serial communication, you need to identify the serial port to which your device is connected. In most cases, it would look like /dev/ttyACM0 for the Coral Dev Board Micro. However, it can differ, especially if you have more devices connected.

Start screen Session: Open a terminal and use the following command, replacing /dev/ttyACM0 with your actual serial port if necessary:

```bash
screen /dev/ttyACM0 115200
```

Here, 115200 specifies the baud rate, a parameter required for establishing the communication link. For the Coral Dev Board Micro, it is typically set to 115200.

Interact with Your Device: If everything is set up correctly, you should now see the output data streaming in your terminal. This real-time data is being sent from your Coral Dev Board Micro, providing invaluable insights into its operations and performance.

Terminating the Session: You can close the screen session anytime by pressing Ctrl-A followed by typing :quit and pressing Enter.

Please note that the availability of serial communication features might depend on the specific permissions and settings of your operating system. Ensure you have the necessary rights or consult your system's documentation if you encounter any issues.

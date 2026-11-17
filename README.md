
# RC 4WD Car with Obstacle Detection and Avoidance

A four-wheel-drive (4WD) remote-controlled robotic car developed using an Arduino Uno, FlySky FS-i6 transmitter, FS-iA6B receiver, L298N motor driver, and HC-SR04 ultrasonic sensor. The vehicle supports wireless manual control with skid-steering operation and automatic forward-motion restriction when an obstacle is detected.

## Project Demonstration

Watch the project demonstration video: [▶ Link](https://youtu.be/cwCUje_lm6g?si=cqj0wN0LCeqgJaN0)

## Key Features

- **4WD Configuration:** Four DC gear motors arranged in a four-wheel-drive configuration.
- **Wireless RC Control:** Manual operation using a FlySky FS-i6 transmitter and FS-iA6B receiver.
- **iBUS Communication:** Receiver communicates with Arduino through the iBUS protocol.
- **Skid-Steering Control:** Independent speed and direction control of the left and right motor groups.
- **Ultrasonic Obstacle Detection:** HC-SR04 sensor measures the distance to objects in front of the vehicle.
- **Forward-Motion Restriction:** Forward throttle is blocked when an obstacle is detected.
- **Reverse and Turning Control:** Reverse movement and steering remain available on detection.
- **PWM Motor Control:** Motor speed is controlled using Arduino PWM outputs.

## Hardware Components

| Component | Quantity | Purpose |
|---|---:|---|
| Arduino Uno | 1 | Main controller |
| L298N Dual H-Bridge Motor Driver | 1 | Controls four DC motors through two parallel motor groups |
| DC Gear Motors (TT Motors) | 4 | Vehicle propulsion |
| FlySky FS-i6 Transmitter | 1 | Wireless manual control |
| FlySky FS-iA6B Receiver | 1 | Receives transmitter commands |
| HC-SR04 Ultrasonic Sensor | 1 | Front obstacle detection |
| 3S LiPo Battery (11.1V nominal) | 1 | Main power source |
| Robot chassis and wheels | 1 set | Mechanical structure |
| Jumper wires | As required | Electrical connections |

## System Architecture

The system follows this control architecture:

```text
FlySky FS-i6 Transmitter
          |
          | Wireless RC Signal
          v
FlySky FS-iA6B Receiver
          |
          | iBUS (D0 / RX)
          v
      Arduino Uno
          |
          |----------------------|
          |                      |
          v                      v
   L298N Motor Driver      HC-SR04 Sensor
          |                      |
          v                      |
   Four DC Gear Motors           |
          ^                      |
          |______________________|
             Distance Feedback
```

## Electrical Connections

### 1. FlySky FS-iA6B Receiver to Arduino Uno

The receiver communicates with the Arduino using its iBUS signal output.

| FS-iA6B Receiver | Arduino Uno |
|---|---|
| iBUS Signal | D0 (RX) |
| VCC | 5V |
| GND | GND |

**Note:** The iBUS signal is connected to the Arduino Uno's hardware UART receive pin (D0).

### 2. HC-SR04 Ultrasonic Sensor to Arduino Uno

| HC-SR04 Pin | Arduino Uno |
|---|---|
| VCC | 5V |
| Trig | D11 |
| Echo | D2 |
| GND | GND |

### 3. L298N Motor Driver to Arduino Uno

| L298N Pin | Arduino Uno |
|---|---|
| ENA | D9 (PWM) |
| IN1 | D3 |
| IN2 | D4 |
| IN3 | D5 |
| IN4 | D6 |
| ENB | D10 (PWM) |
| 5V Logic | 5V |
| GND | GND |

### 4. Motor Connections

The four motors are connected in two parallel groups using a single L298N motor driver. Both motors on each side receive the same control signal, allowing the vehicle to operate using skid-steering principles.

| L298N Output | Connected Motors |
|---|---|
| OUT1 and OUT2 | Left-front and left-rear motors (parallel) |
| OUT3 and OUT4 | Right-front and right-rear motors (parallel) |

### 5. Power Connections

The 3S LiPo battery provides the motor supply through the L298N motor driver's main power input.

| Power Connection | Destination |
|---|---|
| Battery Positive (+) | L298N +12V / motor supply input |
| Battery Negative (−) | L298N GND |
| Arduino GND | Common GND |
| Arduino 5V | Receiver VCC, HC-SR04 VCC, and L298N logic 5V |

All components must share a common ground reference.

**Important:** The L298N onboard 5V regulator must be disabled by removing the 5V-EN jumper before supplying its logic 5V from the Arduino. Never connect two 5V regulator outputs together. The Arduino should be powered from USB or a suitable regulated 5V source. Do not connect the 3S LiPo directly to the Arduino 5V pin.

## Control Logic

### Manual Radio Control

The Arduino reads two receiver channels:

| Receiver Channel | Function |
|---|---|
| CH1 | Steering |
| CH3 | Throttle |

The controller converts the incoming channel values into motor-speed commands.

### Skid-Steering Mechanism

```text
Left Speed  = Throttle + Steering
Right Speed = Throttle - Steering
```

The resulting motor commands are constrained to the permitted PWM range.

### Ultrasonic Obstacle Detection

The HC-SR04 sensor continuously measures the distance to objects in front of the vehicle. The current obstacle-detection threshold is configured to 25 cm. The control behavior is:

| Condition | Vehicle Response |
|---|---|
| No obstacle within threshold | Normal RC control |
| Obstacle detected while moving forward | Forward throttle blocked |
| Obstacle detected while reversing | Reverse movement permitted |
| Obstacle detected while steering | Steering remains permitted |
| Obstacle detected while turning in place | Turning remains permitted |

## Software Requirements

- Arduino IDE
- Arduino Uno board package
- IBusBM Arduino library

### Required Library

The project uses the `IBusBM` library for communication with the FlySky receiver. Install it through:

1. Open Arduino IDE.
2. Navigate to **Sketch → Include Library → Manage Libraries**.
3. Search for `IBusBM`.
4. Install the library.

**Serial Communication Note:** Since the FS-iA6B receiver uses the Arduino Uno's hardware serial port (D0/RX), the same UART is not available for normal Serial Monitor debugging while iBUS communication is active.

## Obstacle Detection Configuration

The obstacle threshold is defined in the Arduino sketch:

```cpp
const int OBSTACLE_DISTANCE = 25;
```

The value represents the distance in centimeters. Adjust this value according to the vehicle's speed, braking characteristics, and physical dimensions.

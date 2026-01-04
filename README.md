# 🌍 GeoGrapher Real-time Monitor

GeoGrapher is a real-time tracking system designed using the **ESP32** microcontroller and the **NEO-6M GPS** module.

## 🛠 Hardware Wiring

Follow the table below to connect the NEO-6M GPS module to your ESP32 board:

| NEO-6M GPS Pin | ESP32 Pin | Description |
| :--- | :--- | :--- |
| **VCC** | 3.3V / 5V | Power Supply |
| **GND** | GND | Ground |
| **TX** | GPIO 16 (RX2) | Data Transmission (GPS to ESP32) |
| **RX** | GPIO 17 (TX2) | Data Reception (ESP32 to GPS) |

---

## 📸 Circuit & Connection Diagrams

<p align="center">
  <img src="https://github.com/user-attachments/assets/850bf20d-5645-4976-9285-fce4fa7a12a5" width="300" alt="GPS Module">
  <img src="https://github.com/user-attachments/assets/86ccc027-64e9-4308-9896-eeac621dbee4" width="600" alt="Wiring Diagram">
</p>

> [!TIP]
> **Important:** Always remember to cross-connect the data pins. The GPS **TX** must go to the ESP32 **RX (GPIO 16)**, and the GPS **RX** must go to the ESP32 **TX (GPIO 17)** for successful communication.

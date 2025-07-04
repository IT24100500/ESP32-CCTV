# ESP32-CAM Surveillance System with OTA Updates

A feature-rich surveillance system using ESP32-CAM that provides live video streaming, snapshot capture, camera controls, and wireless firmware updates.![Visit](https:chpcbook.up.railway.com)

## Key Features

- 📹 **Live Video Streaming**: MJPEG stream with adjustable resolution
- 📸 **Snapshot Capture**: Save high-quality JPEG images
- 💡 **Flashlight Control**: 4-level brightness adjustment (OFF/LOW/MED/HIGH)
- ⚙️ **Camera Settings**:
  - Resolution adjustment (QQVGA to UXGA)
  - Image quality, brightness, contrast controls
  - Mirror/flip options
- 🔄 **Over-the-Air (OTA) Updates**: Secure firmware updates via web interface
- 🌡️ **Temperature Monitoring**: Real-time chip temperature display
- 📱 **Responsive Web UI**: Works on mobile and desktop browsers

## Technical Specifications

- **Hardware**: ESP32-CAM (AI-Thinker module)
- **WiFi Mode**: Access Point (AP) mode for direct connection
- **Web Server**: Dual server architecture (Port 80 for camera, Port 81 for OTA)
- **Security**:
  - Basic authentication for OTA updates
  - Flash control protection
- **Optimizations**:
  - PSRAM utilization for image buffering
  - Efficient MJPEG streaming
  - Low-memory footprint web UI

## Installation & Usage

1. **Flash the firmware** using Arduino IDE or PlatformIO
2. **Connect** to the ESP32-CAM access point
3. **Access the web interface** at `http://192.168.4.1`
4. **Control the camera** through the intuitive web UI

## Project Structure

```
esp32-cam-surveillance/
├── firmware/           # Main Arduino sketch
├── web_ui/            # HTML/CSS/JS interface
├── libraries/         # Required libraries
└── docs/              # Documentation and schematics
```

## Dependencies

- ESP32 Arduino Core
- ESP32 Camera Driver
- WebServer Library
- Update Library
- ESPmDNS

## Contribution

Contributions are welcome! Please open an issue or submit a pull request for any improvements.

## License

MIT License - Free for personal and commercial use

---

This description highlights:
1. The core features and capabilities
2. Technical implementation details
3. Setup instructions
4. Project organization
5. Licensing information

You can enhance it further with:
- Badges for build status/license
- GIF demo of the interface
- Wiring diagrams
- More detailed installation instructions
- Roadmap of planned features

Would you like me to elaborate on any particular section or add more technical details?

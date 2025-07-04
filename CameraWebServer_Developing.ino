#include "esp_camera.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <Update.h>
#include "esp_http_server.h"
#include "web_ui.h"  // Your HTML UI
#include "driver/ledc.h"
#include "esp32-hal-cpu.h"   // For temperatureRead()

// ESP32-CAM AI Thinker pin config
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
#define LED_GPIO_NUM       4  // Flash LED

// WiFi credentials (AP Mode)
const char* ssid = "ESP32-CAM";
const char* password = "12345678";
const char* host = "esp32-cam";

httpd_handle_t camera_httpd = NULL;
WebServer otaServer(81);  // Separate server for OTA on port 81

int flashBrightness = 0;  // 0–255 PWM value

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

// OTA Update handlers
void handleOTAUpdate() {
  otaServer.sendHeader("Connection", "close");
  otaServer.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
  ESP.restart();
}

void handleOTAUpload() {
  HTTPUpload& upload = otaServer.upload();
  if (upload.status == UPLOAD_FILE_START) {
    Serial.printf("Update: %s\n", upload.filename.c_str());
    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    /* flashing firmware to ESP*/
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) { //true to set the size to the current progress
      Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
    } else {
      Update.printError(Serial);
    }
  }
}

void setupOTAServer() {
  // Setup OTA update server
  otaServer.on("/update", HTTP_GET, []() {
    otaServer.sendHeader("Connection", "close");
    otaServer.send(200, "text/html", 
      "<html><body>"
      "<h1>ESP32-CAM OTA Update</h1>"
      "<form method='POST' action='/update' enctype='multipart/form-data'>"
      "<input type='file' name='update'>"
      "<input type='submit' value='Update'>"
      "</form>"
      "</body></html>");
  });
  
  otaServer.on("/update", HTTP_POST, handleOTAUpdate, handleOTAUpload);
  otaServer.begin();
}

// Index HTML page
static esp_err_t index_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, INDEX_HTML, strlen(INDEX_HTML));
}

// Capture a still image
static esp_err_t capture_handler(httpd_req_t *req) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) return httpd_resp_send_500(req);

    if (flashBrightness > 0) {
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, flashBrightness);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
        delay(150);
    }

    httpd_resp_set_type(req, "image/jpeg");
    httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
    esp_err_t res = httpd_resp_send(req, (const char *)fb->buf, fb->len);

    esp_camera_fb_return(fb);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
    return res;
}

// Live MJPEG stream
static esp_err_t stream_handler(httpd_req_t *req) {
    esp_err_t res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if (res != ESP_OK) return res;

    char part_buf[64];
    while (true) {
        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb) {
            Serial.println("Camera capture failed");
            return ESP_FAIL;
        }

        res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        if (res != ESP_OK) break;

        size_t hlen = snprintf(part_buf, sizeof(part_buf), _STREAM_PART, fb->len);
        res = httpd_resp_send_chunk(req, part_buf, hlen);
        if (res != ESP_OK) break;

        res = httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len);
        esp_camera_fb_return(fb);
        if (res != ESP_OK) break;
    }

    return res;
}

// Flash LED brightness handler
static esp_err_t flash_handler(httpd_req_t *req) {
    char* buf;
    size_t buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len <= 1) return httpd_resp_send(req, NULL, 0);

    buf = (char*)malloc(buf_len);
    if (!buf) return httpd_resp_send_500(req);

    char state[8], level[8];
    flashBrightness = 0;

    if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
        if (httpd_query_key_value(buf, "state", state, sizeof(state)) == ESP_OK) {
            if (strcmp(state, "0") == 0) {
                flashBrightness = 0;
            } else if (httpd_query_key_value(buf, "level", level, sizeof(level)) == ESP_OK) {
                flashBrightness = constrain(atoi(level), 0, 255);
            } else {
                flashBrightness = 255;
            }
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, flashBrightness);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
        }
    }

    free(buf);
    return httpd_resp_send(req, NULL, 0);
}

// Camera control handler
static esp_err_t cmd_handler(httpd_req_t *req) {
    char var[32], val[32];
    char* buf;
    size_t len = httpd_req_get_url_query_len(req) + 1;
    if (len <= 1) return httpd_resp_send_404(req);
    buf = (char*)malloc(len);
    if (!buf) return httpd_resp_send_500(req);

    if (httpd_req_get_url_query_str(req, buf, len) == ESP_OK &&
        httpd_query_key_value(buf, "var", var, sizeof(var)) == ESP_OK &&
        httpd_query_key_value(buf, "val", val, sizeof(val)) == ESP_OK) {
        sensor_t *s = esp_camera_sensor_get();
        int value = atoi(val);
        int result = 0;

        if (!strcmp(var, "framesize")) result = s->set_framesize(s, (framesize_t)value);
        else if (!strcmp(var, "quality")) result = s->set_quality(s, value);
        else if (!strcmp(var, "brightness")) result = s->set_brightness(s, value);
        else if (!strcmp(var, "contrast")) result = s->set_contrast(s, value);
        else if (!strcmp(var, "gainceiling")) result = s->set_gainceiling(s, (gainceiling_t)value);
        else if (!strcmp(var, "hmirror")) result = s->set_hmirror(s, value);
        else if (!strcmp(var, "vflip")) result = s->set_vflip(s, value);
        else result = -1;

        free(buf);
        return (result ? httpd_resp_send_500(req) : httpd_resp_send(req, NULL, 0));
    }

    free(buf);
    return httpd_resp_send_404(req);
}

// Send current camera settings as JSON
static esp_err_t status_handler(httpd_req_t *req) {
    sensor_t *s = esp_camera_sensor_get();
    char json[256];
    snprintf(json, sizeof(json),
        "{\"framesize\":%u,\"quality\":%u,\"brightness\":%d,\"contrast\":%d,"
        "\"gainceiling\":%u,\"hmirror\":%u,\"vflip\":%u}",
        s->status.framesize, s->status.quality, s->status.brightness,
        s->status.contrast, s->status.gainceiling, s->status.hmirror, s->status.vflip);

    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, json, strlen(json));
}

// Internal temperature handler
static esp_err_t temp_handler(httpd_req_t *req) {
    int temp = temperatureRead();  // approx temp * 100
    char temp_str[64];
    snprintf(temp_str, sizeof(temp_str), "{\"temperature\": %.2f}", temp / 100.0);

    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, temp_str, strlen(temp_str));
}

// Start HTTP server
void startCameraServer() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;

    httpd_uri_t uris[] = {
        {"/", HTTP_GET, index_handler, NULL},
        {"/capture", HTTP_GET, capture_handler, NULL},
        {"/stream", HTTP_GET, stream_handler, NULL},
        {"/flash", HTTP_GET, flash_handler, NULL},
        {"/control", HTTP_GET, cmd_handler, NULL},
        {"/status", HTTP_GET, status_handler, NULL},
        {"/temp", HTTP_GET, temp_handler, NULL}
    };

    if (httpd_start(&camera_httpd, &config) == ESP_OK) {
        for (auto &uri : uris) {
            httpd_register_uri_handler(camera_httpd, &uri);
        }
    }
}

void setup() {
    Serial.begin(115200);

    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.frame_size = FRAMESIZE_SVGA;
    config.pixel_format = PIXFORMAT_JPEG;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.jpeg_quality = 12;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;

    if (esp_camera_init(&config) != ESP_OK) {
        Serial.println("Camera init failed");
        return;
    }

    // Setup PWM for flash LED - corrected version
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = LEDC_TIMER_1,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .gpio_num = LED_GPIO_NUM,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_1,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&ledc_channel);

    WiFi.softAP(ssid, password);
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());

    // Start mDNS for easy access
    if (!MDNS.begin(host)) {
        Serial.println("Error setting up MDNS responder!");
    }
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("http", "tcp", 81);  // OTA service

    startCameraServer();
    setupOTAServer();
}

void loop() {
    otaServer.handleClient();
    delay(2);  // Allow the CPU to switch to other tasks
}
#include "OV7670.h"
#include <WebSockets.h>
#include <WebSocketsClient.h>
#include <WebSocketsServer.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClient.h>
#include "canvas_htm.h"

const char *ap_ssid     = "Esp32AP";
const char *ap_password = "12345678";

const char *ssid_AP_1 = "Bots2025";
const char *pwd_AP_1  = "12345678";

const char *ssid_AP_2 = "XXXXXXXXX";
const char *pwd_AP_2  = "xxxxxxxxx";

const char *ssid_AP_3 = "XXXXXXXXX";
const char *pwd_AP_3  = "xxxxxxxxx";

const int SIOD = 19; // SDA
const int SIOC = 18; // SCL

const int VSYNC = 16;
const int HREF = 17;

const int XCLK = 5;
const int PCLK = 4;

// Используем только 4-битный интерфейс (D4-D7)

const int D6 = 0;
const int D7 = 2;

const int TFT_DC = 2;
const int TFT_CS = 5;

OV7670 *camera;

WiFiMulti wifiMulti;
WiFiServer server(80);

unsigned char start_flag = 0xAA;
unsigned char end_flag = 0xFF;
unsigned char ip_flag = 0x11;

WebSocketsServer webSocket(81);

void startWebSocket() {
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("WebSocket server started.");
}

void startWebServer() {
   server.begin();
   Serial.println("Http web server started.");
}

void serve() {
  WiFiClient client = server.available();
  if (client) {
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print(canvas_htm);
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
  }  
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t payloadlength) {
  int blk_count = 0;
  char canvas_VGA[] = "canvas-VGA";
  char canvas_Q_VGA[] = "canvas-Q-VGA";
  char canvas_QQ_VGA[] = "canvas-QQ-VGA";
  char canvas_QQQ_VGA[] = "canvas-QQQ-VGA";
  char ipaddr[26];
  IPAddress localip;
  
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        webSocket.sendBIN(0, &ip_flag, 1);
        localip = WiFi.localIP();
        sprintf(ipaddr, "%d.%d.%d.%d", localip[0], localip[1], localip[2], localip[3]);
        webSocket.sendTXT(0, (const char *)ipaddr);
      }
      break;
    case WStype_TEXT:
      if (payloadlength == sizeof(canvas_QQQ_VGA)-1) {
        if (memcmp(canvas_QQQ_VGA, payload, payloadlength) == 0) {
          Serial.printf("canvas_QQQ_VGA");
          webSocket.sendBIN(0, &end_flag, 1);
          // Инициализация камеры в 4-битном режиме для QQQVGA
          camera = new OV7670(OV7670::Mode::QQQVGA_RGB565, SIOD, SIOC, VSYNC, HREF, XCLK, PCLK, -1, -1, -1, -1, -1, -1, D6, D7);
        }
      } else if (payloadlength == sizeof(canvas_QQ_VGA)-1) {
        if (memcmp(canvas_QQ_VGA, payload, payloadlength) == 0) {
          Serial.printf("canvas_QQ_VGA");
          webSocket.sendBIN(0, &end_flag, 1);
          // Инициализация камеры в 4-битном режиме для QQVGA
          camera = new OV7670(OV7670::Mode::QQVGA_RGB565, SIOD, SIOC, VSYNC, HREF, XCLK, PCLK, -1, -1, -1, -1, -1, -1, D6, D7);
        }
      } else if (payloadlength == sizeof(canvas_Q_VGA)-1) {
        if (memcmp(canvas_Q_VGA, payload, payloadlength) == 0) {
          Serial.printf("canvas_Q_VGA");
          webSocket.sendBIN(0, &end_flag, 1);
          // Инициализация камеры в 4-битном режиме для QVGA
          camera = new OV7670(OV7670::Mode::QVGA_RGB565, SIOD, SIOC, VSYNC, HREF, XCLK, PCLK, -1, -1, -1, -1, -1, -1, D6, D7);
        }
      } else if (payloadlength == sizeof(canvas_VGA)-1) {
        if (memcmp(canvas_VGA, payload, payloadlength) == 0) {
          Serial.printf("canvas_VGA");
          webSocket.sendBIN(0, &end_flag, 1);
          // Инициализация камеры в 4-битном режиме для VGA
          camera = new OV7670(OV7670::Mode::VGA_RGB565, SIOD, SIOC, VSYNC, HREF, XCLK, PCLK, -1, -1, -1, -1, -1, -1, D6, D7);
        }
      }

      if (camera != nullptr) {
        blk_count = camera->yres/I2SCamera::blockSlice;
        for (int i=0; i<blk_count; i++) {
          if (i == 0) {
            camera->startBlock = 1;
            camera->endBlock = I2SCamera::blockSlice;
            webSocket.sendBIN(0, &start_flag, 1);
          }

          if (i == blk_count-1) {
            webSocket.sendBIN(0, &end_flag, 1);
          }
        
          camera->oneFrame();
          webSocket.sendBIN(0, camera->frame, camera->xres * I2SCamera::blockSlice * 2);
          camera->startBlock += I2SCamera::blockSlice;
          camera->endBlock += I2SCamera::blockSlice;
        }
      }
      break;
    case WStype_ERROR:
      Serial.printf("Error \n");
    default:
      Serial.printf("WStype %x not handled \n", type);
  }
}

void initWifiMulti() {
  wifiMulti.addAP(ssid_AP_1, pwd_AP_1);
  wifiMulti.addAP(ssid_AP_2, pwd_AP_2);
  wifiMulti.addAP(ssid_AP_3, pwd_AP_3);

  Serial.println("Connecting Wifi...");
  while(wifiMulti.run() != WL_CONNECTED) {
    delay(5000);        
    Serial.print(".");
  }
  
  Serial.print("\nWiFi connected : ");
  Serial.print("IP address : ");
  Serial.println(WiFi.localIP());
}

void initWifiAP() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ap_ssid, ap_password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}

void setup() {
  Serial.begin(115200);
  initWifiMulti();
  initWifiAP();
  startWebSocket();
  startWebServer();
}

void loop() {
  webSocket.loop();
  serve();
}
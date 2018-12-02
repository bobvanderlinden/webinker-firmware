#include <FS.h>
#include <SPIFFS.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <HardwareSerial.h>
#include <ArduinoJson.h>

const char* server_ca= \
"-----BEGIN CERTIFICATE-----\n" \
"MIIEsTCCA5mgAwIBAgIQBOHnpNxc8vNtwCtCuF0VnzANBgkqhkiG9w0BAQsFADBs\n" \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
"d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n" \
"ZSBFViBSb290IENBMB4XDTEzMTAyMjEyMDAwMFoXDTI4MTAyMjEyMDAwMFowcDEL\n" \
"MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\n" \
"LmRpZ2ljZXJ0LmNvbTEvMC0GA1UEAxMmRGlnaUNlcnQgU0hBMiBIaWdoIEFzc3Vy\n" \
"YW5jZSBTZXJ2ZXIgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC2\n" \
"4C/CJAbIbQRf1+8KZAayfSImZRauQkCbztyfn3YHPsMwVYcZuU+UDlqUH1VWtMIC\n" \
"Kq/QmO4LQNfE0DtyyBSe75CxEamu0si4QzrZCwvV1ZX1QK/IHe1NnF9Xt4ZQaJn1\n" \
"itrSxwUfqJfJ3KSxgoQtxq2lnMcZgqaFD15EWCo3j/018QsIJzJa9buLnqS9UdAn\n" \
"4t07QjOjBSjEuyjMmqwrIw14xnvmXnG3Sj4I+4G3FhahnSMSTeXXkgisdaScus0X\n" \
"sh5ENWV/UyU50RwKmmMbGZJ0aAo3wsJSSMs5WqK24V3B3aAguCGikyZvFEohQcft\n" \
"bZvySC/zA/WiaJJTL17jAgMBAAGjggFJMIIBRTASBgNVHRMBAf8ECDAGAQH/AgEA\n" \
"MA4GA1UdDwEB/wQEAwIBhjAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIw\n" \
"NAYIKwYBBQUHAQEEKDAmMCQGCCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdpY2Vy\n" \
"dC5jb20wSwYDVR0fBEQwQjBAoD6gPIY6aHR0cDovL2NybDQuZGlnaWNlcnQuY29t\n" \
"L0RpZ2lDZXJ0SGlnaEFzc3VyYW5jZUVWUm9vdENBLmNybDA9BgNVHSAENjA0MDIG\n" \
"BFUdIAAwKjAoBggrBgEFBQcCARYcaHR0cHM6Ly93d3cuZGlnaWNlcnQuY29tL0NQ\n" \
"UzAdBgNVHQ4EFgQUUWj/kK8CB3U8zNllZGKiErhZcjswHwYDVR0jBBgwFoAUsT7D\n" \
"aQP4v0cB1JgmGggC72NkK8MwDQYJKoZIhvcNAQELBQADggEBABiKlYkD5m3fXPwd\n" \
"aOpKj4PWUS+Na0QWnqxj9dJubISZi6qBcYRb7TROsLd5kinMLYBq8I4g4Xmk/gNH\n" \
"E+r1hspZcX30BJZr01lYPf7TMSVcGDiEo+afgv2MW5gxTs14nhr9hctJqvIni5ly\n" \
"/D6q1UEL2tU2ob8cbkdJf17ZSHwD2f2LSaCYJkJA69aSEaRkCldUxPUd1gJea6zu\n" \
"xICaEnL6VpPX/78whQYwvwt/Tv9XBZ0k7YXDK/umdaisLRbvfXknsuvCnQsH6qqF\n" \
"0wGjIChBWUMo0oHjqvbsezt3tkBigAVBRQHvFwY+3sAzm2fTYS5yh+Rp/BIAV0Ae\n" \
"cPUeybQ=\n" \
"-----END CERTIFICATE-----";

const char *webinkerHostname = "webinker.herokuapp.com";
char webAddress[255] = "https://google.com";

WiFiClient wifiClient;

// HardwareSerial Serial2(2);
RTC_DATA_ATTR int bootCount = 0;

int configurationPin = 4;

void onWiFiManagerAP (WiFiManager *wifiManager) {
  Serial.print("Entering configuration portal as ssid ");
  Serial.println(wifiManager->getConfigPortalSSID());
}

void onWiFiManagerSaveConfig () {
  saveConfig();
}

const char *configFileName = "/config.json";

void loadConfig() {
  if (SPIFFS.begin()) {
    if (SPIFFS.exists(configFileName)) {
      File configFile = SPIFFS.open(configFileName, "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t fileSize = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> fileBuffer(new char[fileSize]);

        configFile.readBytes(fileBuffer.get(), fileSize);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(fileBuffer.get());
        json.printTo(Serial);
        if (json.success()) {
          strncpy(webAddress, json["web_address"], sizeof(webAddress));
        } else {
          Serial.println("Failed to load json config");
        }
        configFile.close();
      } else {
        Serial.println("No configuration file found");
      }
    }
  } else {
    Serial.println("Failed to mount FS");
  }
}

void saveConfig() {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["webAddress"] = webAddress;

    File configFile = SPIFFS.open(configFileName, "w");
    if (!configFile) {
      Serial.println("Failed to open config file for writing");
    } else {
      json.printTo(Serial);
      json.printTo(configFile);
      configFile.close();
    }
}

void setup() {
  Serial.begin(115200);

  pinMode(configurationPin, INPUT);

  Serial.printf("BootCount: %d", bootCount);
  Serial.println();
  bootCount++;

  WiFiManagerParameter webAddressParameter("webAddress", "web address", webAddress, 255);

  WiFiManager wifiManager;
  wifiManager.setAPCallback(onWiFiManagerAP);
  wifiManager.setSaveConfigCallback(onWiFiManagerSaveConfig);
  wifiManager.addParameter(&webAddressParameter);
  if (digitalRead(configurationPin)) {
    wifiManager.startConfigPortal("webinker", "justsendit");
  } else {
    wifiManager.autoConnect("webinker", "justsendit");
  }

  Serial2.begin(1000000, SERIAL_8N1, 16, 17);

  Serial.print("Checking wifi connection");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println();

  Serial.printf("Connected to wifi %s", WiFi.SSID());
  Serial.println();

  Serial.println("Starting connection to server...");
  if (!wifiClient.connect(webinkerHostname, 80))
    Serial.println("Connection failed!");
  else {
    Serial.println("Connected to server!");

    wifiClient.print("GET /?url=");
    wifiClient.print(webAddressParameter.getValue());
    wifiClient.println("&format=gray HTTP/1.0");
    wifiClient.print("Host: ");
    wifiClient.println(webinkerHostname);
    wifiClient.println("Connection: close");
    wifiClient.println();

    while (wifiClient.connected()) {
      String line = wifiClient.readStringUntil('\n');
      wifiClient.println(line);
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }
    Serial.println("Writing body to UART...");

    Serial2.write("\xaa\x55\x40");
    int writtenByteCount = 0;
    while (wifiClient.connected()) {
      while (wifiClient.available()) {
        char c = wifiClient.read();
        Serial2.write(c);        
        writtenByteCount++;
      }
    }
    Serial.printf("Received %d bytes.", writtenByteCount);
    Serial.println();
    while (writtenByteCount < 120000) {
      Serial2.write(0);
      writtenByteCount++;
    }
    Serial2.flush();
    wifiClient.stop();
    Serial.printf("Finished transfering %d bytes.", writtenByteCount);
    Serial.println();
  }


  Serial.println("Going to sleep...");
  Serial.flush();

  esp_sleep_enable_timer_wakeup(5 * 1000000);
  esp_deep_sleep_start();
}

void loop() {
  // do nothing
}
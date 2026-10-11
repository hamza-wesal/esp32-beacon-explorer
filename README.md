# ESP32 802.11 Beacon Frame Explorer

An educational Proof-of-Concept (PoC) demonstrating how the ESP32 can interact with raw 802.11 management frames (`esp_wifi_80211_tx`). 

## Purpose
This project is built strictly for wireless networking research, learning how beacon frames structure SSID payloads, and observing how modern mobile operating systems handle MAC address isolation.

## Features
- Dynamic memory management to prevent payload bleeding across different SSID lengths.
- Spoofing unique source MAC/BSSID pairings to isolate broadcast instances.

# Spotify Music Display

<img width="1919" height="992" alt="image" src="https://github.com/user-attachments/assets/9d16dc34-3218-4785-b625-6e6d9824fc36" />

A compact Spotify "now playing" display built with an ESP32-C3 Super Mini, a 1.8″ ST7735 TFT screen, and three tactile buttons — all inside a custom 3D-printed case.

## Wiring

### TFT (ST7735)

| TFT pin | ESP32-C3 GPIO |
|---------|--------------|
| CS      | 7            |
| DC      | 2            |
| RST     | 3            |
| SDA (MOSI) | 6         |
| SCL (SCLK) | 4         |
| VCC     | 3.3 V        |
| GND     | GND          |

### Buttons (active-LOW, uses internal pull-up)

| Button | ESP32-C3 GPIO |
|--------|--------------|
| PREV   | 0            |
| PLAY/PAUSE | 1        |
| NEXT   | 5            |

---

## Software Setup

### 1. Install Arduino libraries

Open **Arduino IDE → Tools → Manage Libraries** and install:

| Library | Author |
|---------|--------|
| **SpotifyArduino** | Brian Lough |
| **ArduinoJson** | Benoit Blanchon (v6) |
| **TFT_eSPI** | Bodmer |

### 2. Configure TFT_eSPI

In the TFT_eSPI library folder, open `User_Setup.h` and set:

```cpp
#define ST7735_DRIVER
#define TFT_WIDTH  128
#define TFT_HEIGHT 160
#define TFT_CS   7
#define TFT_DC   2
#define TFT_RST  3
#define TFT_MOSI 6
#define TFT_SCLK 4
```

### 3. Get a Spotify Refresh Token

1. Go to <https://developer.spotify.com/dashboard> and create a new app.
2. Set the **Redirect URI** to `http://<ESP_IP>/callback`.
3. Open the SpotifyArduino library example **`get_token`**, enter your Client ID and Secret, and upload it to the board.
4. Follow the serial-monitor instructions to complete the OAuth flow and copy the **refresh token**.

### 4. Edit credentials in the sketch

Open `spotifymusicdisplay.ino` and fill in:

```cpp
#define WIFI_SSID              "YOUR_WIFI_SSID"
#define WIFI_PASSWORD          "YOUR_WIFI_PASSWORD"
#define SPOTIFY_CLIENT_ID      "YOUR_SPOTIFY_CLIENT_ID"
#define SPOTIFY_CLIENT_SECRET  "YOUR_SPOTIFY_CLIENT_SECRET"
#define SPOTIFY_REFRESH_TOKEN  "YOUR_SPOTIFY_REFRESH_TOKEN"
```

### 5. Upload

Select **ESP32C3 Dev Module** (or equivalent) in the Board menu and upload.

---

## Usage

| Button | Action |
|--------|--------|
| PREV   | Previous track |
| PLAY/PAUSE | Toggle playback |
| NEXT   | Next track |

The display refreshes every **5 seconds** to show the currently playing track and artist.
## About the Project

The Spotify Music Display is a compact, custom-built hardware device that shows the currently playing Spotify track on a small 1.8" TFT screen. It features tactile buttons with custom keycaps for playback control and is housed in a 3D-printed enclosure, all powered by an ESP32 microcontroller.

## Why I Made This

I built this project because I wanted a dedicated, always-on display for my music without having to glance at my phone or computer. It was also a great opportunity to combine hardware design, 3D printing, and embedded programming into a single fun project.

## Bill of Materials

| Name | Purpose | Quantity | Total Cost (USD) | Link | Distributor |
|------|---------|----------|-----------------|------|-------------|
| Case printing | 3D-printed enclosure | 1 | $5.33 | — | Printing Legion |
| 7-in-1 60W Soldering Iron Starter Kit | Soldering equipment | 1 | $2.54 | [Link](https://electronicspices.com/product/7-in-1-combo-of-60-watt-soldering-iron-starter-kit-for-project-work) | ElectronicSpices |
| Blank DSA Keycaps | Button keycaps | 3 | $3.19 | [Link](https://meckeys.com/shop/accessories/keyboard-accessories/keycaps/blank-dsa-keycaps-1u/) | MecKeys |
| 25× M3 Heatset Inserts | Fastening parts together | 1 | $0.95 | [Link](https://robu.in/product/m3-x-4-mm-brass-heat-set-knurl-threaded-round-insert-nut-25-pcs/) | Robu |
| 10× Tactile Switches | Button inputs | 1 | $1.91 | [Link](https://www.thecosmicbyte.com/product/kailh-mechanical-switches-for-swappable-keyboards-pack-of-10/) | The Cosmic Byte |
| 1.8" TFT Display (ST7735, 128×160) | Displays Spotify track info | 1 | $1.73 | [Link](https://robokits.co.in/displays/lcd-display/1.8-inch-st7735-tft-lcd-module-with-4-io-128160) | Robokits |
| ESP32-C3 Super Mini Board | Main microcontroller | 1 | $3.00 | [Link](https://hubtronics.in/esp32-c3-super-mini-unsoldered) | Hubtronics |


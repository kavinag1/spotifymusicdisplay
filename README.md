# spotifymusicdisplay

<img width="1919" height="992" alt="image" src="https://github.com/user-attachments/assets/9d16dc34-3218-4785-b625-6e6d9824fc36" />

A compact Spotify "now playing" display built with an ESP32-C3 Super Mini, a 1.8″ ST7735 TFT screen, and three tactile buttons — all inside a custom 3D-printed case.

---

## Bill of Materials

See [`apple music player-bom.csv`](apple%20music%20player-bom.csv) for the full parts list and links.

| Part | Notes |
|------|-------|
| ESP32-C3 Super Mini | Wi-Fi + BLE microcontroller |
| 1.8″ ST7735 TFT (128 × 160) | Display |
| 3 × Tactile switch + DSA keycap | Prev / Play-Pause / Next |
| 25 × M3 heatset inserts | Case assembly |
| 3D-printed case | See `case.step` / `back.step` |

---

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

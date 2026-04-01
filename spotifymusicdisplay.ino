/*
 * Spotify Music Display
 * Hardware: ESP32-C3 Super Mini + 1.8" ST7735 TFT (128x160) + 3 tactile buttons
 *
 * Required Arduino libraries (install via Library Manager):
 *   - SpotifyArduino  by Brian Lough
 *   - ArduinoJson     by Benoit Blanchon (v6)
 *   - TFT_eSPI        by Bodmer
 *
 * TFT wiring (ST7735 128x160):
 *   CS   -> GPIO 7
 *   DC   -> GPIO 2
 *   RST  -> GPIO 3
 *   SDA  -> GPIO 6  (MOSI)
 *   SCL  -> GPIO 4  (SCLK)
 *   VCC  -> 3.3V
 *   GND  -> GND
 *
 * Button wiring (active-LOW, internal pull-up):
 *   PREV -> GPIO 0
 *   PLAY -> GPIO 1
 *   NEXT -> GPIO 5
 *
 * Spotify OAuth setup:
 *   1. Create an app at https://developer.spotify.com/dashboard
 *   2. Set Redirect URI to http://YOUR_ESP_IP/callback
 *   3. Fill in CLIENT_ID, CLIENT_SECRET, and REFRESH_TOKEN below
 *      (use the SpotifyArduino "get_token" example to obtain your refresh token)
 */

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <SpotifyArduino.h>
#include <SpotifyArduinoCert.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include <SPI.h>

// ---------------------------------------------------------------------------
// User configuration – edit these values
// ---------------------------------------------------------------------------
#define WIFI_SSID       "YOUR_WIFI_SSID"
#define WIFI_PASSWORD   "YOUR_WIFI_PASSWORD"

#define SPOTIFY_CLIENT_ID      "YOUR_SPOTIFY_CLIENT_ID"
#define SPOTIFY_CLIENT_SECRET  "YOUR_SPOTIFY_CLIENT_SECRET"
#define SPOTIFY_REFRESH_TOKEN  "YOUR_SPOTIFY_REFRESH_TOKEN"
// ---------------------------------------------------------------------------

// Button GPIO pins
#define BTN_PREV  0
#define BTN_PLAY  1
#define BTN_NEXT  5

// How often to poll Spotify for the current track (milliseconds)
#define POLL_INTERVAL_MS 5000

// Debounce delay (milliseconds)
#define DEBOUNCE_MS 200

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------
TFT_eSPI tft = TFT_eSPI();

WiFiClientSecure client;
SpotifyArduino spotify(client, SPOTIFY_CLIENT_ID, SPOTIFY_CLIENT_SECRET,
                       SPOTIFY_REFRESH_TOKEN);

unsigned long lastPoll   = 0;
unsigned long lastDebounce[3] = {0, 0, 0};

char currentTrack[64]  = "";
char currentArtist[64] = "";
bool isPlaying         = false;

// ---------------------------------------------------------------------------
// Forward declarations
// ---------------------------------------------------------------------------
void connectWifi();
void drawSplashScreen();
void updateDisplay(const char *track, const char *artist, bool playing);
void handleButtons();
void pollSpotify();
void printCurrentlyPlaying(CurrentlyPlaying &cp);

// ---------------------------------------------------------------------------
// Setup
// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);

  // Buttons – internal pull-up; buttons connect pin to GND
  pinMode(BTN_PREV, INPUT_PULLUP);
  pinMode(BTN_PLAY, INPUT_PULLUP);
  pinMode(BTN_NEXT, INPUT_PULLUP);

  // TFT
  tft.init();
  tft.setRotation(1);   // landscape (160x128)
  tft.fillScreen(TFT_BLACK);

  drawSplashScreen();

  connectWifi();

  // Trust Spotify's root CA
  client.setCACert(SPOTIFY_CERTIFICATE);

  // Obtain a fresh access token using the stored refresh token
  Serial.println(F("Refreshing Spotify access token..."));
  if (!spotify.refreshAccessToken()) {
    Serial.println(F("Failed to refresh token – check credentials"));
  }

  lastPoll = millis();
}

// ---------------------------------------------------------------------------
// Main loop
// ---------------------------------------------------------------------------
void loop() {
  handleButtons();

  if (millis() - lastPoll >= POLL_INTERVAL_MS) {
    lastPoll = millis();
    pollSpotify();
  }
}

// ---------------------------------------------------------------------------
// WiFi connection
// ---------------------------------------------------------------------------
void connectWifi() {
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(4, 4);
  tft.print(F("Connecting to WiFi"));

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  uint8_t dot = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    tft.setCursor(4 + dot * 6, 16);
    tft.print(F("."));
    dot = (dot + 1) % 20;
  }

  Serial.print(F("Connected. IP: "));
  Serial.println(WiFi.localIP());

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(4, 4);
  tft.print(F("WiFi OK"));
  delay(800);
  tft.fillScreen(TFT_BLACK);
}

// ---------------------------------------------------------------------------
// Splash screen
// ---------------------------------------------------------------------------
void drawSplashScreen() {
  tft.fillScreen(TFT_BLACK);
  // Green Spotify-style circle
  tft.fillCircle(80, 50, 28, TFT_GREEN);
  tft.fillCircle(80, 50, 20, TFT_BLACK);
  tft.fillCircle(80, 50, 12, TFT_GREEN);

  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(18, 88);
  tft.print(F("Spotify"));

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(26, 110);
  tft.print(F("Music Display"));

  delay(1500);
  tft.fillScreen(TFT_BLACK);
}

// ---------------------------------------------------------------------------
// Display: currently playing track
// ---------------------------------------------------------------------------
void updateDisplay(const char *track, const char *artist, bool playing) {
  tft.fillScreen(TFT_BLACK);

  // Status icon (▶ or ‖)
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(4, 4);
  tft.print(playing ? ">" : "||");

  // Track name
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(4, 28);
  tft.print(F("Track:"));
  tft.setCursor(4, 40);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  // Truncate to fit 160-px wide display (approx 26 chars at size 1)
  char truncTrack[27];
  strncpy(truncTrack, track, 26);
  truncTrack[26] = '\0';
  tft.print(truncTrack);

  // Artist name
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(4, 60);
  tft.print(F("Artist:"));
  tft.setCursor(4, 72);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  char truncArtist[27];
  strncpy(truncArtist, artist, 26);
  truncArtist[26] = '\0';
  tft.print(truncArtist);

  // Bottom button hints
  tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
  tft.setCursor(0, 116);
  tft.print(F("|<  PREV |PLAY/PAUSE| NEXT >|"));
}

// ---------------------------------------------------------------------------
// Button handling
// ---------------------------------------------------------------------------
void handleButtons() {
  unsigned long now = millis();

  // PREV
  if (digitalRead(BTN_PREV) == LOW && now - lastDebounce[0] > DEBOUNCE_MS) {
    lastDebounce[0] = now;
    Serial.println(F("PREV pressed"));
    spotify.previousTrack();
    delay(300);
    pollSpotify();
  }

  // PLAY / PAUSE
  if (digitalRead(BTN_PLAY) == LOW && now - lastDebounce[1] > DEBOUNCE_MS) {
    lastDebounce[1] = now;
    Serial.println(F("PLAY/PAUSE pressed"));
    if (isPlaying) {
      spotify.pause();
    } else {
      spotify.play();
    }
    // Let the next poll update the true state from the API
    delay(300);
    pollSpotify();
  }

  // NEXT
  if (digitalRead(BTN_NEXT) == LOW && now - lastDebounce[2] > DEBOUNCE_MS) {
    lastDebounce[2] = now;
    Serial.println(F("NEXT pressed"));
    spotify.skipToNext();
    delay(300);
    pollSpotify();
  }
}

// ---------------------------------------------------------------------------
// Spotify polling
// ---------------------------------------------------------------------------
void pollSpotify() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(F("WiFi lost – reconnecting..."));
    connectWifi();
  }

  CurrentlyPlaying cp = spotify.getCurrentlyPlaying();

  if (cp.error) {
    Serial.println(F("Error fetching current track – refreshing token"));
    spotify.refreshAccessToken();
    return;
  }

  printCurrentlyPlaying(cp);

  // Update local state only when something has changed
  bool changed = false;
  const char *newArtist = (cp.numArtists > 0) ? cp.artists[0].artistName : "";
  if (strcmp(currentTrack,  cp.trackName) != 0 ||
      strcmp(currentArtist, newArtist)    != 0 ||
      isPlaying != cp.isPlaying) {
    changed = true;
  }

  if (changed) {
    strncpy(currentTrack,  cp.trackName, sizeof(currentTrack)  - 1);
    strncpy(currentArtist, newArtist,    sizeof(currentArtist) - 1);
    currentTrack[sizeof(currentTrack)   - 1] = '\0';
    currentArtist[sizeof(currentArtist) - 1] = '\0';
    isPlaying = cp.isPlaying;
    updateDisplay(currentTrack, currentArtist, isPlaying);
  }
}

// ---------------------------------------------------------------------------
// Debug helper
// ---------------------------------------------------------------------------
void printCurrentlyPlaying(CurrentlyPlaying &cp) {
  Serial.print(F("Track : ")); Serial.println(cp.trackName);
  if (cp.numArtists > 0) {
    Serial.print(F("Artist: ")); Serial.println(cp.artists[0].artistName);
  }
  Serial.print(F("Playing: ")); Serial.println(cp.isPlaying ? "yes" : "no");
}


// WiFi
#define CONFIG_WIFI_SSID "Project14"
#define CONFIG_WIFI_PASS "B@ssettwifi2018"

// MQTT
#define CONFIG_MQTT_HOST "192.168.1.65"
#define CONFIG_MQTT_PORT 1883 // Usually 1883
#define CONFIG_MQTT_USER "clock"
#define CONFIG_MQTT_PASS "clock"
#define CONFIG_MQTT_CLIENT_ID "ben_wall_clock_1" // Must be unique on the MQTT network


// MQTT Topics
#define CONFIG_MQTT_TOPIC_STATE "home/clock"
#define CONFIG_MQTT_TOPIC_SET "home/clock/set"

#define CONFIG_MQTT_PAYLOAD_ON "ON"
#define CONFIG_MQTT_PAYLOAD_OFF "OFF"

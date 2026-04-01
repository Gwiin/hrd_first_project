#ifndef NODE_C_CONFIG_H
#define NODE_C_CONFIG_H

/*
 * Team-friendly local defaults.
 * If you set WIFI_SSID / WIFI_PASSWORD / MQTT_SERVER as compile definitions
 * or environment variables in CMake, those values override these defaults.
 */

#define NODE_C_DEFAULT_WIFI_SSID "bindsoft"
#define NODE_C_DEFAULT_WIFI_PASSWORD "bindsoft24"
#define NODE_C_DEFAULT_MQTT_SERVER "163.152.213.111"

/*
 * Optional MQTT authentication.
 * Leave empty if your broker allows anonymous access.
 */
#define NODE_C_DEFAULT_MQTT_USERNAME ""
#define NODE_C_DEFAULT_MQTT_PASSWORD ""

#define NODE_C_DEFAULT_MQTT_PORT 1883
#define NODE_C_DEFAULT_MQTT_KEEP_ALIVE_S 60
#define NODE_C_DEFAULT_MQTT_QOS 1

#endif

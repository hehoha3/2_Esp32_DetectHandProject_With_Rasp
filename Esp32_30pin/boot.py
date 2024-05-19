import ubinascii
import machine
import micropython
import network

import esp
esp.osdebug(None)

import gc
gc.collect()


# Connection Information
WIFI_SSID = "pepu"
WIFI_PASSWORD = "88888888"
MQTT_HOST     = '192.168.1.5'

CLIENT_ID     = ubinascii.hexlify(machine.unique_id())
TOPIC_SUB     = b'home/ledControl'

# Connect Wifi
print("Connecting to Wi-Fi...")
station       = network.WLAN(network.STA_IF)
station.active(True)
station.connect(WIFI_SSID, WIFI_PASSWORD)

while station.isconnected() == False:
  pass

print('Connection successful')
print("IP address: ", station.ifconfig()[0])
print(station.ifconfig())
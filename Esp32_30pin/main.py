import time
import machine
from umqttsimple import MQTTClient

# pins
relay = machine.Pin(15, machine.Pin.OUT)

# Subscribe callback Func
def sub_cb(topic, msg):
    print(f"Message received on topic {topic}: {msg}")
    if msg == b'A':
        relay.value(1)
        print("Relay is now ON!")
    elif msg == b'B':
        relay.value(0)
        print("Relay is now OFF!")
    elif msg == b'L':
        print("Testing")

# Connect and subscribe to MQTT topic
def connect_and_subscribe():
  global CLIENT_ID, MQTT_HOST, TOPIC_SUB
  client = MQTTClient(CLIENT_ID, MQTT_HOST)
  client.set_callback(sub_cb)
  client.connect()
  client.subscribe(TOPIC_SUB)
  print('Connected to %s MQTT broker, subscribed to %s topic' % (MQTT_HOST, TOPIC_SUB))
  return client

def restart_and_reconnect():
  print('Failed to connect to MQTT broker. Reconnecting...')
  time.sleep(10)
  machine.reset()

try:
  client = connect_and_subscribe()
except OSError as e:
  restart_and_reconnect()

while True:
    try:
        client.wait_msg()  # Đợi thông điệp mới từ MQTT
    except Exception as e:
        print("Error in MQTT loop: ", e)
        time.sleep(2)
        connect_and_subscribe()
    time.sleep(0.1)
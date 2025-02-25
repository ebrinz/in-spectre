import json
import paho.mqtt.client as mqtt
import time
from typing import Dict, Any

# MQTT configuration - adjust based on your setup
MQTT_BROKER = "localhost"
MQTT_PORT = 1883
MQTT_TOPIC_AS7263 = "sensor/as7263"
MQTT_TOPIC_AS7265X = "sensor/as7265x"

# Cache for latest sensor readings
sensor_cache = {
    "as7263": {},
    "as7265x": {}
}
last_update_time = {
    "as7263": 0,
    "as7265x": 0
}

# MQTT client setup
def setup_mqtt_client():
    client = mqtt.Client()
    
    def on_connect(client, userdata, flags, rc):
        print(f"Connected with result code {rc}")
        client.subscribe(MQTT_TOPIC_AS7263)
        client.subscribe(MQTT_TOPIC_AS7265X)
    
    def on_message(client, userdata, msg):
        try:
            payload = json.loads(msg.payload.decode())
            
            if msg.topic == MQTT_TOPIC_AS7263:
                sensor_cache["as7263"] = payload
                last_update_time["as7263"] = time.time()
            elif msg.topic == MQTT_TOPIC_AS7265X:
                sensor_cache["as7265x"] = payload
                last_update_time["as7265x"] = time.time()
        except Exception as e:
            print(f"Error processing message: {e}")
    
    client.on_connect = on_connect
    client.on_message = on_message
    
    try:
        client.connect(MQTT_BROKER, MQTT_PORT, 60)
        client.loop_start()
    except Exception as e:
        print(f"Failed to connect to MQTT broker: {e}")
    
    return client

# Initialize MQTT client
mqtt_client = setup_mqtt_client()

def get_as7263_data() -> Dict[str, Any]:
    """Get the latest data from AS7263 sensor via MQTT."""
    current_time = time.time()
    cache_age = current_time - last_update_time["as7263"]
    
    if cache_age > 30:  # If data is older than 30 seconds
        raise Exception("Sensor data is stale or unavailable")
    
    return {
        "timestamp": last_update_time["as7263"],
        "data": sensor_cache["as7263"]
    }

def get_as7265x_data() -> Dict[str, Any]:
    """Get the latest data from AS7265x sensor via MQTT."""
    current_time = time.time()
    cache_age = current_time - last_update_time["as7265x"]
    
    if cache_age > 30:  # If data is older than 30 seconds
        raise Exception("Sensor data is stale or unavailable")
    
    return {
        "timestamp": last_update_time["as7265x"],
        "data": sensor_cache["as7265x"]
    }
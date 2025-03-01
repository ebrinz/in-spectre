import json
import serial
import time
import threading
from typing import Dict, Any, Optional
import logging

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("serial_service")

# Cache for latest sensor readings
sensor_cache = {
    "as7263": {},
    "as7265x": {}
}
last_update_time = {
    "as7263": 0,
    "as7265x": 0
}

# Serial connection
serial_port = None
serial_thread = None
is_running = False

def open_serial_connection(port: str, baud_rate: int = 115200) -> bool:
    """Open serial connection to the ESP32 device."""
    global serial_port, is_running
    
    try:
        serial_port = serial.Serial(port, baud_rate, timeout=1)
        logger.info(f"Connected to {port} at {baud_rate} baud")
        is_running = True
        return True
    except Exception as e:
        logger.error(f"Failed to open serial port: {e}")
        return False

def close_serial_connection() -> None:
    """Close the serial connection."""
    global serial_port, is_running
    
    is_running = False
    if serial_port and serial_port.is_open:
        serial_port.close()
        logger.info("Serial connection closed")

def serial_reader_thread() -> None:
    """Thread function to continuously read from serial port."""
    global serial_port, is_running
    
    logger.info("Serial reader thread started")
    
    while is_running and serial_port and serial_port.is_open:
        try:
            if serial_port.in_waiting:
                line = serial_port.readline().decode('utf-8').strip()
                
                # Skip debug/info messages
                if line and line[0] == '{':
                    try:
                        data = json.loads(line)
                        if "sensor" in data:
                            sensor_type = data["sensor"]
                            if sensor_type == "AS7263":
                                # Update cache for AS7263
                                sensor_cache["as7263"] = data
                                last_update_time["as7263"] = time.time()
                                logger.debug("Updated AS7263 data")
                            elif sensor_type == "AS7265X":
                                # Update cache for AS7265X
                                sensor_cache["as7265x"] = data
                                last_update_time["as7265x"] = time.time()
                                logger.debug("Updated AS7265X data")
                    except json.JSONDecodeError:
                        logger.warning(f"Invalid JSON: {line}")
            else:
                # Small delay to prevent CPU hogging
                time.sleep(0.01)
        except Exception as e:
            logger.error(f"Error reading from serial: {e}")
            time.sleep(1)  # Delay before retry
    
    logger.info("Serial reader thread stopped")

def start_serial_service(port: str, baud_rate: int = 115200) -> bool:
    """Start the serial service."""
    global serial_thread
    
    if open_serial_connection(port, baud_rate):
        # Start the reader thread
        serial_thread = threading.Thread(target=serial_reader_thread, daemon=True)
        serial_thread.start()
        return True
    return False

def stop_serial_service() -> None:
    """Stop the serial service."""
    global is_running, serial_thread
    
    is_running = False
    close_serial_connection()
    
    if serial_thread and serial_thread.is_alive():
        serial_thread.join(timeout=2.0)
        if serial_thread.is_alive():
            logger.warning("Serial thread did not stop cleanly")

def get_as7263_data() -> Dict[str, Any]:
    """Get the latest data from AS7263 sensor via serial."""
    current_time = time.time()
    cache_age = current_time - last_update_time["as7263"]
    
    if cache_age > 30:  # If data is older than 30 seconds
        raise Exception("Sensor data is stale or unavailable")
    
    return {
        "timestamp": last_update_time["as7263"],
        "data": sensor_cache["as7263"]
    }

def get_as7265x_data() -> Dict[str, Any]:
    """Get the latest data from AS7265x sensor via serial."""
    current_time = time.time()
    cache_age = current_time - last_update_time["as7265x"]
    
    if cache_age > 30:  # If data is older than 30 seconds
        raise Exception("Sensor data is stale or unavailable")
    
    return {
        "timestamp": last_update_time["as7265x"],
        "data": sensor_cache["as7265x"]
    }

def list_available_ports() -> list:
    """List available serial ports."""
    try:
        from serial.tools import list_ports
        return [port.device for port in list_ports.comports()]
    except ImportError:
        logger.error("serial.tools.list_ports not available")
        return []
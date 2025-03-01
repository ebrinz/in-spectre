#!/usr/bin/env python3
"""
In-Spectre - Command-line runner script
"""
import os
import sys
import argparse
from api import create_app

def parse_args():
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(description='Run the In-Spectre Flask API')
    parser.add_argument('--debug', action='store_true', help='Run in debug mode')
    parser.add_argument('--host', default='127.0.0.1', help='Host to bind to')
    parser.add_argument('--port', type=int, default=5000, help='Port to bind to')
    
    # Communication options
    comm_group = parser.add_argument_group('Communication Options')
    comm_group.add_argument('--mode', choices=['mqtt', 'serial'], default='mqtt',
                          help='Communication mode: mqtt or serial')
    
    # MQTT options
    mqtt_group = parser.add_argument_group('MQTT Options')
    mqtt_group.add_argument('--mqtt-broker', default='localhost',
                          help='MQTT broker address (default: localhost)')
    mqtt_group.add_argument('--mqtt-port', type=int, default=1883,
                          help='MQTT broker port (default: 1883)')
    
    # Serial options
    serial_group = parser.add_argument_group('Serial Options')
    serial_group.add_argument('--serial-port',
                            help='Serial port (e.g., /dev/ttyUSB0 or COM3)')
    serial_group.add_argument('--serial-baud', type=int, default=115200,
                            help='Serial baud rate (default: 115200)')
    
    return parser.parse_args()

def main():
    """Run the Flask application."""
    args = parse_args()
    
    # Create app configuration
    config = {
        'COMMUNICATION_MODE': args.mode,
        'MQTT_BROKER': args.mqtt_broker,
        'MQTT_PORT': args.mqtt_port,
        'SERIAL_PORT': args.serial_port,
        'SERIAL_BAUD_RATE': args.serial_baud
    }
    
    # Create the app with config
    app = create_app(config)
    
    # Run the app
    app.run(
        debug=args.debug,
        host=args.host,
        port=args.port
    )

if __name__ == '__main__':
    main()
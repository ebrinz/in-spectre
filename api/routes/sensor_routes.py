from flask import Blueprint, jsonify, request, current_app
import importlib

bp = Blueprint('sensors', __name__, url_prefix='/api/sensors')

def get_service_module():
    """Dynamically get the correct service module based on current configuration."""
    mode = current_app.config.get('COMMUNICATION_MODE', 'mqtt')
    
    if mode == 'mqtt':
        return importlib.import_module('api.services.spectral_service')
    elif mode == 'serial':
        return importlib.import_module('api.services.serial_service')
    else:
        raise ValueError(f"Unsupported communication mode: {mode}")

@bp.route('/status', methods=['GET'])
def status():
    """Check if API is running and get the current communication mode."""
    return jsonify({
        "status": "online",
        "communication_mode": current_app.config.get('COMMUNICATION_MODE', 'mqtt')
    })

@bp.route('/mode', methods=['GET'])
def get_mode():
    """Get the current communication mode."""
    return jsonify({
        "mode": current_app.config.get('COMMUNICATION_MODE', 'mqtt')
    })

@bp.route('/mode', methods=['POST'])
def set_mode():
    """Set the communication mode and parameters."""
    data = request.get_json()
    if not data or 'mode' not in data:
        return jsonify({"error": "Missing mode parameter"}), 400
    
    mode = data['mode']
    if mode not in ['mqtt', 'serial']:
        return jsonify({"error": "Invalid mode. Must be 'mqtt' or 'serial'"}), 400
    
    # Update app config
    current_app.config['COMMUNICATION_MODE'] = mode
    
    # Additional parameters for each mode
    if mode == 'mqtt':
        if 'broker' in data:
            current_app.config['MQTT_BROKER'] = data['broker']
        if 'port' in data:
            current_app.config['MQTT_PORT'] = int(data['port'])
        
        # Reconnect MQTT
        from api.services.spectral_service import setup_mqtt_client
        mqtt_client = setup_mqtt_client()
    
    elif mode == 'serial':
        # Stop any existing serial connection
        try:
            from api.services.serial_service import stop_serial_service
            stop_serial_service()
        except:
            pass
        
        if 'port' in data:
            current_app.config['SERIAL_PORT'] = data['port']
        if 'baud_rate' in data:
            current_app.config['SERIAL_BAUD_RATE'] = int(data['baud_rate'])
        
        # Start serial if port is provided
        if current_app.config['SERIAL_PORT']:
            from api.services.serial_service import start_serial_service
            success = start_serial_service(
                current_app.config['SERIAL_PORT'],
                current_app.config['SERIAL_BAUD_RATE']
            )
            if not success:
                return jsonify({"error": "Failed to open serial port"}), 500
    
    return jsonify({
        "success": True,
        "mode": mode,
        "config": {
            "mqtt_broker": current_app.config.get('MQTT_BROKER'),
            "mqtt_port": current_app.config.get('MQTT_PORT'),
            "serial_port": current_app.config.get('SERIAL_PORT'),
            "serial_baud_rate": current_app.config.get('SERIAL_BAUD_RATE')
        }
    })

@bp.route('/ports', methods=['GET'])
def list_ports():
    """List available serial ports."""
    try:
        from api.services.serial_service import list_available_ports
        ports = list_available_ports()
        return jsonify({"ports": ports})
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@bp.route('/as7263', methods=['GET'])
def as7263_data():
    """Get spectral data from AS7263 sensor."""
    try:
        service = get_service_module()
        data = service.get_as7263_data()
        return jsonify(data)
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@bp.route('/as7265x', methods=['GET'])
def as7265x_data():
    """Get spectral data from AS7265x sensor."""
    try:
        service = get_service_module()
        data = service.get_as7265x_data()
        return jsonify(data)
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@bp.route('/all', methods=['GET'])
def all_sensor_data():
    """Get data from all spectral sensors."""
    try:
        service = get_service_module()
        
        as7263 = service.get_as7263_data()
        as7265x = service.get_as7265x_data()
        
        return jsonify({
            "as7263": as7263,
            "as7265x": as7265x
        })
    except Exception as e:
        return jsonify({"error": str(e)}), 500
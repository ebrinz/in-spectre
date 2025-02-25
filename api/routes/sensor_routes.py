from flask import Blueprint, jsonify, request
from api.services.spectral_service import get_as7263_data, get_as7265x_data

bp = Blueprint('sensors', __name__, url_prefix='/api/sensors')

@bp.route('/status', methods=['GET'])
def status():
    """Check if API is running."""
    return jsonify({"status": "online"})

@bp.route('/as7263', methods=['GET'])
def as7263_data():
    """Get spectral data from AS7263 sensor."""
    try:
        data = get_as7263_data()
        return jsonify(data)
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@bp.route('/as7265x', methods=['GET'])
def as7265x_data():
    """Get spectral data from AS7265x sensor."""
    try:
        data = get_as7265x_data()
        return jsonify(data)
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@bp.route('/all', methods=['GET'])
def all_sensor_data():
    """Get data from all spectral sensors."""
    try:
        as7263 = get_as7263_data()
        as7265x = get_as7265x_data()
        
        return jsonify({
            "as7263": as7263,
            "as7265x": as7265x
        })
    except Exception as e:
        return jsonify({"error": str(e)}), 500
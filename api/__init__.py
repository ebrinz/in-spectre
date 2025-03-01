from flask import Flask
from flask_cors import CORS

def create_app(config=None):
    app = Flask(__name__)
    CORS(app)
    
    # Default configuration
    app.config.update(
        COMMUNICATION_MODE='mqtt',  # 'mqtt' or 'serial'
        MQTT_BROKER='localhost',
        MQTT_PORT=1883,
        SERIAL_PORT=None,  # Set to device path like '/dev/ttyUSB0' or 'COM3'
        SERIAL_BAUD_RATE=115200
    )
    
    # Override with custom config if provided
    if config:
        app.config.update(config)
    
    # Initialize communication service based on configuration
    if app.config['COMMUNICATION_MODE'] == 'mqtt':
        # Set up MQTT client
        from api.services.spectral_service import setup_mqtt_client
        mqtt_client = setup_mqtt_client()
    elif app.config['COMMUNICATION_MODE'] == 'serial' and app.config['SERIAL_PORT']:
        # Set up Serial connection
        from api.services.serial_service import start_serial_service
        start_serial_service(
            app.config['SERIAL_PORT'],
            app.config['SERIAL_BAUD_RATE']
        )
    
    # Register routes
    from api.routes import sensor_routes
    app.register_blueprint(sensor_routes.bp)
    
    # Register error handlers
    @app.errorhandler(404)
    def not_found(error):
        return {'error': 'Not found'}, 404
    
    @app.errorhandler(500)
    def server_error(error):
        return {'error': 'Internal server error'}, 500
    
    return app
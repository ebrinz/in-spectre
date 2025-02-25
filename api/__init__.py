from flask import Flask
from flask_cors import CORS

def create_app():
    app = Flask(__name__)
    CORS(app)
    
    from api.routes import sensor_routes
    app.register_blueprint(sensor_routes.bp)
    
    return app
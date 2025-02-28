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
    return parser.parse_args()

def main():
    """Run the Flask application."""
    args = parse_args()
    app = create_app()
    
    # Run the app
    app.run(
        debug=args.debug,
        host=args.host,
        port=args.port
    )

if __name__ == '__main__':
    main()
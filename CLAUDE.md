# CLAUDE.md - Guidelines for in-spectre

## Build & Test Commands
- Setup virtual env: `python -m venv venv && source venv/bin/activate`
- Install dependencies: `pip install -r requirements.txt`
- Run API server: `flask run --debug`
- Lint Python: `flake8 api/`
- Type check: `mypy api/`
- Run tests: `pytest tests/`
- Run single test: `pytest tests/test_file.py::test_function -v`
- Upload Arduino code: `arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:avr:uno ./firmware`

## Code Style Guidelines
- **Python API**:
  - Follow PEP 8 conventions
  - Use type hints for all functions
  - Group imports: stdlib, third-party, local
  - Snake_case for functions/variables, PascalCase for classes
  - Comprehensive docstrings (Google style)
- **Arduino**:
  - Descriptive snake_case function names
  - UPPER_SNAKE_CASE for constants
  - Comment sensor calibration values
  - Consistent error handling with LED indicators
- **Sensor Protocol**: Document clear data format specs for Arduino→API communication
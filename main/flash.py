import os
import sys
import subprocess
import argparse
import platform

def get_idf_path():
    """Get the ESP-IDF path from environment variable."""
    idf_path = os.environ.get('IDF_PATH')
    if not idf_path:
        raise EnvironmentError("IDF_PATH is not set. Please set up ESP-IDF environment.")
    return idf_path

def get_python_path():
    """Get the Python executable path."""
    return sys.executable

def run_command(command):
    """Run a shell command and return its output."""
    try:
        result = subprocess.run(command, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        return result.stdout
    except subprocess.CalledProcessError as e:
        print(f"Error executing command: {e}")
        print(f"Error output: {e.stderr}")
        sys.exit(1)

def flash_esp32s3(port, baud_rate, project_path):
    """Flash the ESP32S3 with the project at the given path."""
    idf_path = get_idf_path()
    python_path = get_python_path()
    
    # Determine the correct script extension based on the OS
    script_ext = '.bat' if platform.system() == 'Windows' else '.sh'
    
    # Build the project
    print("Building project...")
    build_command = [
        os.path.join(idf_path, f'export{script_ext}'),
        '&&',
        'idf.py',
        '-C', project_path,
        'build'
    ]
    run_command(build_command)
    
    # Flash the project
    print("Flashing ESP32S3...")
    flash_command = [
        os.path.join(idf_path, f'export{script_ext}'),
        '&&',
        'idf.py',
        '-C', project_path,
        '-p', port,
        '-b', str(baud_rate),
        'flash'
    ]
    run_command(flash_command)
    
    print("Flashing completed successfully!")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Flash ESP32S3 with a project.")
    parser.add_argument("--port", required=True, help="Serial port for the ESP32S3")
    parser.add_argument("--baud", type=int, default=115200, help="Baud rate for flashing (default: 115200)")
    parser.add_argument("--project", required=True, help="Path to the project directory")
    
    args = parser.parse_args()
    
    flash_esp32s3(args.port, args.baud, args.project)
# ECG Processing Application

This application is containerized using Docker for easy development and deployment.

## Prerequisites

### Windows Users
1. Install Docker Desktop
2. Install VcXsrv Windows X Server:
   - Download from: https://sourceforge.net/projects/vcxsrv/
   - Or install via winget: `winget install marha.VcXsrv`

### Linux Users
1. Install Docker and Docker Compose
2. X11 is usually pre-installed

### macOS Users
1. Install Docker Desktop
2. Install XQuartz:
   ```bash
   brew install --cask xquartz
   ```

## Setup Instructions

### Windows Setup
1. After installing VcXsrv, launch XLaunch:
   - Choose "Multiple windows" in the first screen
   - Set "Display number" to 0
   - Select "Start no client"
   - Check these options:
     - "Disable access control"
     - "Native opengl"
   - Save the configuration (optional but recommended)

2. Clone the repository:
   ```bash
   git clone [repository-url]
   cd [repository-name]
   ```

3. Build the application:
   ```bash
   docker compose build --no-cache
   ```

## Running the Application

1. Make sure your X server (VcXsrv/XQuartz) is running

2. Start the application:
   ```bash
   docker compose up ecg-dev
   ```

3. For Qt Designer (UI editing):
   ```bash
   docker compose up qt-designer
   ```

4. To run tests:
   ```bash
   docker compose up ecg-test
   ```

## Troubleshooting

### Common Issues

1. "Cannot connect to X server" error:
   - Verify X server (VcXsrv/XQuartz) is running
   - Check firewall settings
   - Restart X server

2. Blank window:
   - Ensure "Native opengl" is checked in XLaunch settings
   - Try adding `-ac` to XLaunch arguments

3. Build cache issues:
   ```bash
   # Clear Docker build cache
   docker builder prune
   
   # Remove project's build cache
   docker volume rm ecg-app_build-cache
   
   # Full system cleanup (use with caution)
   docker system prune -a --volumes
   ```

4. After pulling new changes:
   ```bash
   # Rebuild the containers
   docker compose build --no-cache
   ```

## Development

- Source code is mounted in the container at `/home/user/project`
- Build artifacts are stored in a Docker volume for persistence
- UI files can be edited using Qt Designer

## Project Structure

```
├── src/                  # Source files
├── include/              # Header files
├── tests/                # Test files
├── Dockerfile           # Docker configuration
├── docker-compose.yml   # Docker Compose configuration
└── CMakeLists.txt      # CMake build configuration
```

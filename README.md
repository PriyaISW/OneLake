# OneLake ODBC Test Applications

This repository contains test applications for ODBC connectivity with Microsoft OneLake using the Simba OneLake ODBC driver.

## Applications

1. **ConsecutiveConnection_Issue.cpp** - Demonstrates a consecutive connection issue scenario
2. **ConsecutiveConnection_Successful.cpp** - Demonstrates the proper way to handle consecutive connections with full disconnect

## Platform Support

These applications support:
- **Windows** (x86, x64)
- **Linux RHEL8** (x86_64, ARM64/aarch64)

## Prerequisites

### RHEL8 ARM64 / x86_64

1. **Install Development Tools:**
   ```bash
   sudo dnf groupinstall "Development Tools"
   sudo dnf install gcc-c++ make
   ```

2. **Install unixODBC:**
   ```bash
   sudo dnf install unixODBC unixODBC-devel
   ```

3. **Install Simba OneLake ODBC Driver:**
   - Download the appropriate driver for your architecture from the Microsoft/Simba website
   - For ARM64, ensure you download the ARM64-compatible driver
   - Follow the installation instructions provided with the driver

4. **Configure ODBC DSN:**
   Edit `/etc/odbc.ini` or `~/.odbc.ini` to add your DSN configuration:
   ```ini
   [Simba Onelake DSN]
   Driver=/path/to/simba/onelake/driver/lib/libonelakeodbc.so
   # Add other required configuration parameters
   ```

### Windows

1. Install Visual Studio with C++ development tools
2. Install the Simba OneLake ODBC driver for Windows
3. Configure the ODBC DSN using ODBC Data Source Administrator

## Building

### RHEL8 (ARM64 / x86_64)

```bash
# Build all applications
make

# Build a specific application
make ConsecutiveConnection_Issue
make ConsecutiveConnection_Successful

# Check build information
make info

# Clean build artifacts
make clean
```

### Windows

Use Visual Studio to build the applications or compile from command line:
```cmd
cl /EHsc ConsecutiveConnection_Issue.cpp odbc32.lib
cl /EHsc ConsecutiveConnection_Successful.cpp odbc32.lib
```

## Running

### Linux
```bash
./ConsecutiveConnection_Issue
./ConsecutiveConnection_Successful
```

### Windows
```cmd
ConsecutiveConnection_Issue.exe
ConsecutiveConnection_Successful.exe
```

## Architecture-Specific Notes

### ARM64 on RHEL8
- Ensure you have the ARM64 version of the Simba OneLake ODBC driver installed
- The Makefile automatically detects the architecture (`aarch64` for ARM64)
- No special compilation flags are needed for ARM64; the code is architecture-independent

### Troubleshooting

1. **Cannot find ODBC headers:**
   ```bash
   sudo dnf install unixODBC-devel
   ```

2. **Driver not found at runtime:**
   - Verify driver path in `/etc/odbc.ini` or `~/.odbc.ini`
   - Check that the driver library is accessible: `ldd /path/to/driver.so`

3. **Architecture mismatch:**
   - Verify driver architecture: `file /path/to/driver.so`
   - Should show `ARM aarch64` for ARM64 systems

## Code Portability

The code has been made platform-independent:
- Windows-specific headers (`windows.h`) are conditionally included
- String types and ODBC functions adapt based on platform (SQLWCHAR for Windows, SQLCHAR for Linux)
- ODBC API calls use standard ODBC 3.x functions available on all platforms

## License

Please refer to the repository license file for licensing information.

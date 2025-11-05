# Windows API Memory Scanner - Process Memory Digger
<!-- //madebybunnyrce -->

Native C++ memory scanning tool that digs through other processes' memory like a nosy bastard. Uses OpenProcess and ReadProcessMemory APIs to read whatever the hell is stored in other applications' memory space. Great for demonstrating how memory scanning actually works.

## How This Memory Scanner Works

This shit uses several Windows APIs to dig through process memory:

- **Process Enumeration**: Uses `EnumProcesses()` to find all the running processes
- **Process Access**: Uses `OpenProcess()` to get permission to read their memory
- **Memory Reading**: Uses `ReadProcessMemory()` to actually scan through memory regions
- **Memory Mapping**: Uses `VirtualQueryEx()` to figure out which memory regions we can actually read

Basically gives you direct access to whatever other applications have stored in memory.

## Files in This Package

- `capture.cpp` - Memory scanner using OpenProcess/ReadProcessMemory
- `receiver.cpp` - Network receiver for memory scan results
- `build.bat` - Batch script with Windows API build configuration
- `config.ini` - Memory scanning configuration settings
- `README.md` - This documentation file

## How to Use This Memory Scanner

1. **Compile with Windows SDK:**
   ```cmd
   build.bat
   ```
2. **Run receiver first:**
   ```cmd
   receiver.exe
   ```
3. **Start memory scanner:**
   ```cmd
   capture.exe
   ```

## Technical Details

This implementation demonstrates memory scanning techniques:

- **OpenProcess()** - Opens process handles for memory access
- **ReadProcessMemory()** - Reads data from target process memory
- **VirtualQueryEx()** - Queries memory region information
- **Process enumeration** - Scans multiple processes systematically
- **Memory pattern matching** - Searches for specific data patterns

## API Signatures

This implementation uses these Windows APIs:
- OpenProcess() for process handle acquisition
- ReadProcessMemory() for memory data extraction
- VirtualQueryEx() for memory region analysis
- EnumProcesses() for process discovery
- Process32First/Next() for process iteration

## Requirements

- Windows 10/11 with appropriate permissions
- Visual Studio with Windows SDK
- Administrator privileges for memory access
- Debug privileges (SeDebugPrivilege) for system processes

## Limitations

- **Permission dependency** - requires appropriate access rights
- **Memory protection** - some regions may be inaccessible
- **Process privileges** - system processes require debug privileges
- **Performance impact** - memory scanning is resource intensive
- **Access patterns** - creates detectable memory access signatures

## Usage Notes

This is for **educational and development purposes**. Memory scanning techniques are used by debuggers, profilers, and system monitoring tools for legitimate purposes.

---

**Note**: Memory scanning APIs are commonly monitored by security software due to their use in both legitimate debugging tools and malicious software.
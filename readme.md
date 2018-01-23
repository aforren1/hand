
To use via PlatformIO:

1. Install platformio-ide (via visual studio code or atom?)
    - The Arduino IDE + Teensyduino *could* work, but it's a bit more effort to get C++11/STL support
2. clone this directory (`git clone https://github.com/aforren1/hand`)
3. Open the directory in the text editor of choice
4. Hit Ctrl+Shift+P and find "PlatformIO: New Terminal"
5. In that terminal, type `platformio init --board teensy35`
6. To compile & link, `platformio run`
7. To upload to the board, `platformio run --target upload`
8. To clean up files (sometimes helps in development), `platformio run --target clean`

Notes:
 - Make sure Teensy is up to date (`platformio platform update`), but let me know if you're compiling on Windows (woes in Windows + Visual Studio Code)
 - setters return an integer (0 on success, > 0 on failure).
   - We can either keep these vague (i.e. "Your last command failed"), or get specific.
 - We should use namespaces excessively.
 - Get local versions of all the req libs; we want to be independent of build systems
 - Assume all incoming communication is big-endian, and send all outgoing communication as big-endian. 
   - Note that the teensy is little-endian.
 - If a unit needs to maintain a state, it's implemented as a `struct`/`class`. Otherwise, we use a namespace.
   - analog, calibration, communication, constants, packing (TODO: finish) are examples of namespaces.
   - multiplexer and settings use structs (multiplexer cares about the current selection).
 - Compiler flags to turn toggle HID/Serial communication, hardware support
 - Note that `int` == `short` on Arduino, and there's some room for ambiguity there (the C++ linter seems to be interpreting `int` as having 32 bits). Should we just use `int16_t` and co?
 - Generate docs via `doxygen Doxyfile` in the top-level directory.

```python
b'a' # change to acquire mode (sampling) (only from config mode)
b'c' # change to config mode (no sampling) (only from acquire mode)
# can only do the following in config mode:
b'sf' + struct.pack('>f', 1000.0) # set the sampling frequency to 1000 Hz
b'sm' + struct.pack('B', 1) # turn on game mode
b'sg' + struct.pack('>bbbf', -1, -1, 3, 48.0) # set the product of gains across all fingers & channels to 48.0
b'gg' + struct.pack('bbb', 0, 1, 2) # see how that changed the first finger, second channel, output gain

```

To use via PlatformIO:

1. Install platformio-ide (via visual studio code or atom?)
    - The Arduino IDE + Teensyduino *could* work, but it's a bit more effort to get C++11/standard library support
2. clone this directory (`git clone --recursive https://github.com/aforren1/hand`)
3. Open the directory in the text editor of choice
4. Hit Ctrl+Shift+P and find "PlatformIO: New Terminal"
5. In that terminal, type `platformio init --board teensy35`
6. To compile & link, `platformio run`
7. To upload to the board, `platformio run --target upload` (add `-e teensy35_serial_dbg` to target the serial env)
8. To clean up files (sometimes helps in development), `platformio run --target clean`

Notes:
 - Make sure Teensy is up to date (`platformio platform update`), but let me know if you're compiling on Windows (woes in Windows + Visual Studio Code)
 - Generate docs via `doxygen Doxyfile` in the top-level directory.

 - setters return an integer (0 on success, > 0 on failure).
   - We can either keep these vague (i.e. "Your last command failed"), or get specific.
 - Get local versions of all the req libs; we want to be independent of build systems
 - If a unit needs to maintain a state, it's implemented as a `struct`/`class`. Otherwise, we use a namespace.
   - analog, calibration, communication, constants, packing (TODO: finish) are examples of namespaces.
   - we use a struct for the settings.
 - On various Linux distributions, you'll need to run `sudo cp inst/49-teensy.rules /etc/udev/rules.d/49-teensy.rules` (see `inst/49-teensy.rules` for details).

## Commands:

### Config mode
- set ('s')
  - sampling frequency ('f'), a float (0, 1000], default `100`
  - game mode ('m'), truthy or falsy, default `false`
    - Whether the conversion to cartesian coordinates happens on the device (T) or not (F)
  - verbosity ('v'), truthy or falsy, default `false`
    - Whether to send messages about the details of various operations inside the code (T) or not (F)
  - gain ('g'), depending on the targeted gain
    - Third element in the buffer is the target finger, [-1, 4] (-1 sets all fingers)
    - Fourth element in the buffer is the target channel, [-1, 3] (-1 sets all channels)
    - Fifth element in the buffer is the target gain, [0, 3]
      - 0 is the front-end gain, 1 is the fine gain, 2 is the output gain, and 3 is a product of the previous
  - The last 4 elements are the float value, bounds determined by the particular gain being modified
- get ('g')
  - All of the above (except gain operates on a single finger/channel/gain at a time)
  - last error code ('e'), to see whether/why the previous command failed
- acquisition ('a')
  - Change to acquisition mode
- tuning ('t') (sorry, need a new char)
  - Run calibration on all channels (without moving to acquisition mode)
- data ('d')
  - Single sample reading across all channels, using the predefined game mode
### Acquisition mode
- config ('c')
  - Change to config mode

Below are examples of packets that might be sent, and the results:

```python
b'a' # change to acquisition mode (sampling) (only from config mode)
b'c' # change to config mode (no sampling) (only from acquisition mode)
# can only do the following in config mode:
b'sf' + struct.pack('>f', 1000.0) # set the sampling frequency to 1000 Hz
b'sm' + struct.pack('B', 1) # turn on game mode
b'sg' + struct.pack('>bbbf', -1, -1, 3, 48.0) # set the product of gains across all fingers & channels to 48.0
b'gg' + struct.pack('bbb', 0, 1, 2) # see how that changed the first finger, second channel, output gain

```
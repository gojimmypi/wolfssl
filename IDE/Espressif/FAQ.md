### unable to open ftdi device with description

```
Warn : Interface already configured, ignoring
none separate
VisualGDB_OpenOCD_Ready
Info : Listening on port 6666 for tcl connections
Info : Listening on port 53641 for telnet connections
Error: libusb_open() failed with LIBUSB_ERROR_NOT_FOUND
Error: unable to open ftdi device with description 'Tigard V1.1', serial '*' at bus location '*'
target/esp_common.cfg:4: Error: 
at file "target/esp_common.cfg", line 4
```

Run Zadig to set drivers to `libusbK`. See [JTAG Single Step Debugging ESP32 with Tigard](https://gojimmypi.github.io/Tigard-JTAG-SingleStep-Debugging-ESP32/)

### Panic: assert failed: tcpip_send_msg_wait_sem

```text
assert failed: tcpip_send_msg_wait_sem /IDF/components/lwip/lwip/src/api/tcpip.c:449 (
Invalid mbox)
Setting breakpoint at 0x40081d1d and returning...
```
Check that there's a TCP/IP stack configured.


### Version of the component "wolfssl/wolfssl" satisfying the spec was not found
```
$ idf.py create-project-from-example "wolfssl/wolfssl^5.6.0-stable:wolfssl_benchmark"
Executing action: create-project-from-example
ERROR: Version of the component "wolfssl/wolfssl" satisfying the spec "^5.6.0-stable" was not found.
```

This error will typically occur for a few minutes after the ESP Registry is updated. Wait a it and try again.


[RSA peripheral 50% slower on ESP32-S3/C3 than S2](https://www.esp32.com/viewtopic.php?t=23830)

[ GPIO6,GPIO7,GPIO8,and GPIO9 changed for ESP32-WROOM-32E](https://esp32.com/viewtopic.php?t=29058)

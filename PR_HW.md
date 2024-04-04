# Description

Although labeled as an Arduino update, others may find the new memory allocation messages useful. The included breadcrumbs allowed me to tune wolfSSL `user_settings.` to have TLS client or server examples with as little as 32KB free RAM. (specifically tested on the Arduino Nano 33 IoT)

Ready for Arduino publish version `5.6.6-Arduino.2`

- Enhances the Arduino Library publishing script. All lower case name: `wolfssl`. Move `wolfssl.h` to separate file.
- Updates Arduino client and server examples.
- Allows `WOLFSSL_MSG()` to send messages via Arduino `Serial.print()`. 
- Revises default Arduino `user_settings.h` for wolfSSL. 
- Include a variety of breadcrumb messages when `DEBUG_WOLFSSL` is enabled, mostly memory allocation related.
- Adds some testsuite files to `.gitignore`


Fixes zd# n/a

# Testing

How did you test?

Manually tested in Arduino. 



# Checklist

 - [ ] added tests
 - [ ] updated/added doxygen
 - [ ] updated appropriate READMEs
 - [ ] Updated manual and documentation

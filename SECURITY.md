Security of your ESP-LINK installation 
======================================

This document describes the way to secure your esp-link device.
The security we imply is access over the network, not physical security :
protection of your data against an intruder with access to the hardware is not our scope.
Therefore, we consider the network port (the ESP's WiFi interface) as the interface to be secured,
not the connection between the ESP and the attached microcontroller.

# Interfaces to be secured

- telnet bridge
- web configuration
- OTA programming interface
- esp-link web server
- application interfaces provided by the maker

# Securing the telnet bridge

As from esp-link 3.0, the telnet bridge can be secured in various ways.
All of the measures below can be applied to either of the two ports, independently.

- the port number can be changed (default port numbers are 23 and 2323)
- a port can be disabled
- it can be password protected
- access can be encrypted via SSL (this doesn't work yet at the time of this writing)

# Securing the web configuration

# Securing the ESP OTA programming interface

# Securing the esp-link web server

# Securing application interfaces provided by the maker


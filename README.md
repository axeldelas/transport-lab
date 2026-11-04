# TransportLab - Reliable Transport Protocols

## :book: Overview

University project consisting in the implementation of reliable data transfer protocols in C.

The objective was to design and evaluate transport-layer protocols featuring different reliability mechanisms including error detection, acknowledgements, retransmissions, flow control, and sliding windows.

## :dart: Objectives

- Implement reliable transport protocols in C
- Simulate data transmission over an unreliable network
- Detect corrupted messages
- Recover from packet losses
- Implement flow control mechanisms
- Evaluate protocol performance

## :gear: Technologies

- C
- Linux
- TCP/IP Concepts
- Network Programming

## :hammer_and_wrench: Implemented Protocols

### v0 - Unreliable Transfer

Data transfer without reliability guarantees.

### v1 - Stop-and-Wait with Error Detection

- XOR checksum
- ACK / NACK
- Flow control

### v2 - Stop-and-Wait ARQ

- Error detection
- Retransmission timers
- Packet loss recovery

### v3 - Go-Back-N

- Sliding window
- Cumulative acknowledgements
- Window retransmission

### v4 - Selective Repeat

- Sliding window
- Packet buffering
- Selective retransmissions
- Multiple timers

## :key: Key Concepts

- Error Detection
- Flow Control
- ARQ Protocols
- Sliding Windows
- Sequence Numbers
- Timers
- Reliability Engineering

## :man_technologist: Skills Demonstrated

- C Programming
- Network Protocol Design
- Algorithm Development
- Troubleshooting
- Systems Programming
- Network Engineering

## :books: Documentation

Additional documentation is available in the `/docs` folder.
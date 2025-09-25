
<div align="center">
 
# Nx_AgentQ — Qoraal-powered Embedded Agent <br> Azure RTOS / NetX Duo

</div>

## Overview

Nx_AgentQ demonstrates how to embed a lightweight AI/automation “agent” on an STM32 microcontroller using Qoraal.
It runs on Azure RTOS (ThreadX) with NetX Duo providing TCP/IP networking, and exposes a small HTTP/JSON API so other systems (or your scripts) can talk to the device.

### Typical uses:

- Serve a minimal REST API for status/telemetry.
- Receive commands/events and drive on-device logic via Qoraal state machines.

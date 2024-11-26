
## <b>USBPD_DRP_UX_DRD_HID Application Description</b>

This application is an USBPD type C dual role power (DRP) and dual role data (DRD) application using FreeRTOS.
USBPD DRP: This application initialize the type C port 1 in toggling source and sink modes with only one PDO at 5V.

### <b>USBPD_DRP_UX_DRD_HID Overview</b>

At startup STM32H5xx devices that allow to run a simulation of UCPD scenarios based on Cortex CM33 Core.
To enable USBPD traces, add "_TRACE" to the project preprocessor and use STM32CubeMonUSBPD to display traces.

This application demonstrates the functionalities of the Dual Role Power (DRP) and Dual Role Data (DRD) of the USB-PD
protocol.

- The USB-PD part of the application firmware aims to describe how USB-PD, is
implemented in context of STM32H5xx devices, embedding UCPD IP.

- STM32H573I-DK support Dual Role Power (DRP, i.e. meaning it can establish
a PD contract either in Provider/Source (SRC) or Consumer/Sink (SNK) mode).

- STM32H573I-DK support Dual Role Data (DRD, i.e. meaning it can establish
a PD contract either in Host or Device mode).

- LCD is used to display current USB-PD state of each port.

- The touch screen is used to exit the USB-PD menu when no USB cable is plugged in or to navigate between the mouse
menu and the PD-Commande menu when the board is in device mode.

- The touch screen is used as direction and left mouse button when the board is in MOUSE mode or to navigate between
menus.

- Application implementation is based on Azure RTOS ThreadX.

- When a USB Type-C cable is connected to a port, and a Explicit Contract is established, information displayed on LCD
indicates the Port Role within the contract, i.e. HOST/SRC (Provider/Source) or DEVICE/SNK (Consumer/Sink).

When a Provider is connected to the Port:

- The STM32 MCU behaves as a Consumer (Sink mode),it waits for Power Capabilities message
from the attached Provider. When a Source Capabilities message is received, the STM32
starts the evaluation of the received capabilities and check if one of the received power
objects can meet its power requirement.

The STM32 shall send the Request message to request the new power level from the offered
Source Capabilities.
Once the Explicit Contract is achieved, information that the Port
is then acting as Consumer (SNK) is displayed on LCD.

- When the board is in Sink mode, the board will automatically act as a Device.

When a Consumer is connected to the Port:

- The STM32 MCU behaves as a Provider (Source mode), it exchanges Power profiles with
the connected device and waits for Power Request message from the attached Consumer.
If the requested power can be met, the communication will end with an Accept message.

- Once a Explicit Contract is established, information that the Port is then acting as Provider (SRC) is displayed on
LCD.

- When the board is in Source mode, the board will automatically act as a Host.

When the board is in Host mode :

- A menu will appear with 4 buttons and the device name below :

- Data Role Swap

- Power Role Swap

- Get Source(or Sink) Capa

- Return

When the board is in Device mode :

- The Board in mouse mode will be launched (the touch screen will move the cursor)

- If the "Switch" button is clicked, a menu will be displayed with 4 buttons :

- Data Role Swap

- Power Role Swap

- Get Source(or Sink) Capa

- Return

The Power Role Swap feature is supported (as requiring Dual Role Power support) :

- When connected to an USB-C provider only device (source mode), the power role swap between the two boards is not
possible, DRP board will act automatically as a sink.

- When connected to an USB-C consumer only device (sink mode), the power role swap between the two boards is not
possible, DRP board will act automatically as a source.

- When connected to an USB-C with DRP, the Power Role Swap could be triggered by tapping the Power Role Swap menu.

- If the Power Role Swap request is accepted:

- Power change will be assigned, if the board is in Source mode it will be in Sink mode.

- Power change will be assigned, if the board is in Sink mode it will be in Source mode.

- If the Power Role Swap request is rejected:

- A Rejection message will be displayed.

The Data Role Swap feature is supported (as requiring Dual Role Data support) :

- When the board is in Source mode, the board will automatically act as a Host.

- When the board is in Sink mode, the board will automatically act as a Device.

- The board can be used as a mouse.

- The board can be used as a PD menu to send DataRoleSwap, PowerRoleSwap, GetSinkCapa or GetSourceCapa request.

- The Data Role Swap could be triggered by tapping the Data Role Swap menu.

- If the Data Role Swap request is accepted:

- Role change will be assigned, if the board is in Host mode it will be in Device mode.

- Role change will be assigned, if the board is in Device mode it will be in Host mode.

- If the Data Role Swap request is rejected:

- A Rejection message will be displayed.

The Get Source Capabilities feature is supported :

- When the board is in Sink mode :

- Get Source Capabilities can be triggered by pressing the Get Source Capa menu.

- Provider Capabilities will be displayed on the LCD screen by sending a request (Get_Source_Capa):

- If the request is rejected, a Rejection message will be displayed.

- If the request is accepted, the capabilities of the Provider will be displayed.


The Get Sink Capabilities feature is supported :

- When the board is in Source mode :

- Get Sink Capabilities can be triggered by pressing the Get Sink Capa menu.

- Consumer Capabilities will be displayed on the LCD screen by sending a request (Get_Sink_Capa):

- If the request is rejected, a Rejection message will be displayed.

- If the request is accepted, the capabilities of the Provider will be displayed.


#### <b>Notes</b>

1. Care must be taken when using HAL_Delay(), this function provides accurate
delay (in milliseconds) based on variable incremented in HAL time base ISR.
This implies that if HAL_Delay() is called from a peripheral ISR process, then
the HAL time base interrupt must have higher priority (numerically lower) than
the peripheral interrupt. Otherwise the caller ISR process will be blocked.
To change the HAL time base interrupt priority you have to use HAL_NVIC_SetPriority()
function.

2. The application needs to ensure that the HAL time base is always set to 1 millisecond
to have correct HAL operation.


### <b>Keywords</b>
Azure rtos, USBX, USB, HID, USB-PD


### <b>Hardware and Software environment</b>

- This application runs on STM32H573xx devices.

- This application has been tested with STM32H573I-DK boards revision MB1677-C01 and
can be easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

- Open your preferred toolchain
- Rebuild all files and load your image into target memory
- Run the application



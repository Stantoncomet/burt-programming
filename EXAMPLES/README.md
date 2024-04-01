# Examples
Some examples of code we use for the robot. Examples try to be heavily commented, with a minimumal ammount of code.

Current examples are:
- Wired MAX485 Communication
- Wireless Communication


## Wired MAX485 Communication
Projects used for this example are `MAX485_tx` for sending data and `MAX485_rx` for receiving. Data can only be sent one-way in this example.

**Reference Links**
- [How to use MAX485 with Arduino](https://www.circuitstate.com/tutorials/what-is-rs-485-how-to-use-max485-with-arduino-for-reliable-long-distance-serial-communication/)
- [MAX485 Data Sheet](https://www.analog.com/media/en/technical-documentation/data-sheets/MAX1487-MAX491.pdf)

### Overview
We will be sending data using the MAX485 chip, which helps carry data over long distances. The chip has an input, input enable, output, output enable, power, ground, `A`, and `B`. `A` and `B` are the ports actually doing the communication shenanigans, they connect to the other `A` and `B` on the second chip.

![image](https://www.circuitstate.com/wp-content/uploads/2022/09/RS485-Serial-Communication-Tutorial-with-Arduino-and-MAX485-Pinout-Diagram-CIRCUITSTATE-Electronics-01-768x584.png)

![image](https://github.com/Stantoncomet/burt-programming/assets/96146851/af2ec19e-61b8-4e9f-9ec1-ba68c496b85c)


## Wireless Communication
Examples `wireless_tx` and `wireless_rx`, respectively.

**Required Libraries**
- [RadioHead](https://github.com/adafruit/RadioHead)
  - Dependacy: [SPIFlash_LowPowerLab](https://github.com/LowPowerLab/SPIFlash)

**Reference Links**
- [Adafruit Hookup Guide](https://learn.adafruit.com/adafruit-rfm69hcw-and-rfm96-rfm95-rfm98-lora-packet-padio-breakouts/arduino-wiring)
- [Radio Usage](https://learn.adafruit.com/adafruit-rfm69hcw-and-rfm96-rfm95-rfm98-lora-packet-padio-breakouts/using-the-rfm69-radio)
- [Sparkfun Hookup Guide](https://learn.sparkfun.com/tutorials/rfm69hcw-hookup-guide)

### Overview
To can wirelessly communicate between arduinos using RFM69WCH transcievers, you will need the following materials:
- 2x Arduinos
- 2x Usb-A to Usb-B cables (for programming)
- 14x jumper wires or whatever works
- Antena wire, see [Sparkfun Guide](https://learn.sparkfun.com/tutorials/rfm69hcw-hookup-guide#the-antenna), 22 gage, can be of any metal

### Wiring (Using Adafruit Radios)
![image](https://github.com/Stantoncomet/burt-programming/assets/96146851/26187de8-2eea-44db-a53a-5d178eb8dcbb)

The aqua jumper wire on the right is the antena, you (hopefully) have real wire. All connetions are necessary and must be wired properly for the radios to work. From the Adafruit hookup guide:
 - **Vin** connects to the Arduino **5V** pin. If you're using a 3.3V Arduino, connect to **3.3V**.
 - **GND** connects to Arduino ground.
 - **SCLK** connects to SPI clock. On Arduino Uno/Duemilanove/328-based, thats **Digital 13**. On Mega's, its **Digital 52** and on Leonardo/Due its **ICSP-3**.
 - **MISO** connects to SPI MISO. On Arduino Uno/Duemilanove/328-based, thats **Digital 12**. On Mega's, its **Digital 50** and on Leonardo/Due its **ICSP-1**.
 - **MOSI** connects to SPI MOSI. On Arduino Uno/Duemilanove/328-based, thats **Digital 11**. On Mega's, its **Digital 51** and on Leonardo/Due its **ICSP-4**.
 - **CS** connects to our SPI Chip Select pin. We'll be using **Digital 4** but you can later change this to any pin.
 - **RST** connects to our radio reset pin. We'll be using **Digital 2** but you can later change this pin too.
 - **G0 (IRQ)** connects to an interrupt-capable pin. We'll be using **Digital 3** but you can later change this pin too. However, it must connect a hardware Interrupt pin.

### Running the Code
On a computer, boot up the two examples in the Arduino IDE, and plug one of the arduinos in. Select that arduino in the `wireless_tx` window's port menu, and note that window. This will be the transmitting radio.

![image](https://github.com/Stantoncomet/burt-programming/assets/96146851/4863e985-73e8-4c41-b30c-2b806884eaa2)

Now connect the other arduino and select that one on the `wireless_rx` window's port menu.

![image](https://github.com/Stantoncomet/burt-programming/assets/96146851/58e26214-f854-4c3c-9ef7-f55c7d5f70b3)

Upload both programs and enjoy! Adjust the example code as needed.

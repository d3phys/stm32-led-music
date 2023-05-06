# Led Music :notes:

Led music on stm32f051 and led strip ws2812b.
![alt text](images/light-stripe.png "Light Music")

## Prerequisites & Connection

- stm32f051 microcontroller + programmer
- led strip ws2812b, 144 leds
- mini-jack connector

Make sure to use the correct GPIO pins for your specific microcontroller.
The following pin map for is used for stm32f051 discovery:

- PA2 - ws2812b stripe input channel
- PA4 and PA5 - controller ADC input: minijack left and right channels.

![alt text](images/connection.png "Connection")

## Build

Project should be built from source.

Clone repository:
```
git clone https://github.com/d3phys/stm32-led-music.git
```
Compile project:
```
make
```
Connect stm32 programmer and flash stm32:
```
make flash
```

If you have connected properly, after turning on music you should see led strip blinking.
## Design

Project has three modules:
- ADC + mini-jack (embedded in stm32)
- Core (stm32 itself)
- Led strip (ws2812b)

Procedure:
1. Analogus sound signal from mini-jack is converted to digital via ADC. DMA transfers ADC data to ADC buffer.

2. Core converts digital data from ADC buffer to PWM buffer.
PWM buffer is a set of 24-bit packs, which encode led colors.

3. Led strip driver recieves PWM buffer and configures 16-bit timer and DMA to send PWM signal to led strip.

Using the DMA allows system to work asynchronously, which removes bugs with losing input signal and led strip flickering. However, amount of SRAM (8kib) disallows using large led strips, because of PWM buffer size.
## Authors

- Denis Dedkov ([d3phys](https://github.com/d3phys))
- Alexander Simankovich ([futherus](https://github.com/futherus))


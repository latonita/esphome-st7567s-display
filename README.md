# ST7567 I2C/IIC display (ST7567S, GM12864-59N) component for ESPHome
https://github.com/latonita/esphome-st7567s-display/

*This is temporary solution as of 2023/12 until this driver included into Esphome base components (https://github.com/esphome/esphome/pull/5952).*
**Component already merged into `dev` branch of EspHome, next step - it will be released to wide audience with new EspHome release.**


This is ESPHome component to support I2C display from Aliexpress/Ebay.
Sellers often sell it as: 12864 IIC 4P LCD Module 128X64 I2C ST7567S COG Graphic Display Screen Board LCM Panel 128x64 Dot Matrix Screen for Arduino

![Display itself](images/GM12864-59N.png)


Add to your YAML config: 

```
external_components:
  source: github://latonita/esphome-st7567s-display@main
  components: [st7567s]
```

Example configuration for NodeMCU (esp8266) [`st7567s-display-test.yaml`](st7567s-display-test.yaml).

Over I²C
--------

Connect ``SDA`` and ``SCL`` pins on a module to pins you chose for the :ref:`I²C Bus <i2c>`. 
If your display module has ``RESET`` pin you may optionally connect it to a pin on the 
ESP which may improve reliability. For power, connect ``VCC`` to 3.3V and ``GND`` to ``GND``.


```
    # Example minimal configuration entry
    i2c:
      sda: D1
      scl: D2

    display:
      - platform: st7567_i2c
        id: my_display
        lambda: |-
          it.print(0, 0, id(my_font), "Hello World!");
          
    interval:
      - interval: 1h
        then:
          - lambda: id(my_display).request_refresh();
```

Configuration variables:
************************

- **address** (*Optional*, int): Manually specify the :ref:`I²C <i2c>` address of the display. Defaults to 0x3F.
- **i2c_id** (*Optional*, :ref:`config-id`): Manually specify the ID of the :ref:`I²C Component <i2c>` if you want
  to use multiple I²C buses.
- **reset_pin** (*Optional*, :ref:`Pin Schema <config-pin_schema>`): The RESET pin. Defaults to not connected.
- **id** (*Optional*, :ref:`config-id`): Manually specify the ID used for code generation.
- **update_interval** (*Optional*, :ref:`config-time`): The interval to re-draw the screen. Defaults to ``1s``.
- **pages** (*Optional*, list): Show pages instead of a single lambda. See :ref:`display-pages`.
- **lambda** (*Optional*, :ref:`lambda <config-lambda>`): The lambda to use for rendering the content on the display.
- **invert_colors** (*Optional*, boolean): Display hardware color invesion. Defaults to ``false``.
- **rotation** (*Optional*): Set the rotation of the display. Everything you draw in ``lambda:`` will be rotated
  by this option. One of ``0°`` (default), ``90°``, ``180°``, ``270°``.
- **transform** (*Optional*): Transform the display presentation using hardware. All defaults are ``false``.
   - **mirror_x** (*Optional*, boolean): If true, mirror the physical X axis.
   - **mirror_y** (*Optional*, boolean): If true, mirror the physical Y axis.


**Speeding up the bus:** To speed up the display update process you can select higher I²C frequencies, for example:

```

    # Example increased I²C bus speed 
    i2c:
      sda: D1
      scl: D2
      frequency: 400kHz
```

**Hardware rotation:** 180 degree rotation can be implemented in hardware as in the following example:

```

    # Example using transform section to achieve 180° rotation using hardware
    display:
      - platform: st7567_i2c
        id: my_display
        transform:
          mirror_x: true
          mirror_y: true
        lambda: |-
          it.print(0, 0, id(my_font), "Hello World!");
```


Over SPI
--------

Connect ``D0`` to the ``CLK`` pin you chose for the :ref:`SPI bus <spi>`, connect ``D1`` to the ``MOSI`` pin and ``DC`` and ``CS``
to some GPIO pins on the ESP. For power, connect ``VCC`` to 3.3V and ``GND`` to ``GND``. 
Optionally you can also connect the ``RESET`` pin to a pin on the ESP which may improve reliability.

```
    # Example minimal configuration entry
    spi:
      clk_pin: D1
      mosi_pin: D2

    display:
      - platform: st7567_spi
        id: my_display
        dc_pin: D3
        lambda: |-
          it.print(0, 0, id(my_font), "Hello World!");

    interval:
      - interval: 1h
        then:
          - lambda: id(my_display).request_refresh();
```

Configuration variables:
************************

- **dc_pin** (**Required**, :ref:`Pin Schema <config-pin_schema>`): The DC pin.
- **cs_pin** (*Optional*, :ref:`Pin Schema <config-pin_schema>`): The Chip Select (CS) pin.
- **reset_pin** (*Optional*, :ref:`Pin Schema <config-pin_schema>`): The RESET pin. Defaults to not connected.
- **spi_id** (*Optional*, :ref:`config-id`): Manually specify the ID of the :ref:`SPI Component <spi>` if you want
  to use multiple SPI buses.
- **id** (*Optional*, :ref:`config-id`): Manually specify the ID used for code generation.
- **update_interval** (*Optional*, :ref:`config-time`): The interval to re-draw the screen. Defaults to ``1s``.

- **pages** (*Optional*, list): Show pages instead of a single lambda. See :ref:`display-pages`.
- **lambda** (*Optional*, :ref:`lambda <config-lambda>`): The lambda to use for rendering the content on the display.
- **invert_colors** (*Optional*, boolean): Display hardware color invesion. Defaults to ``false``.
- **rotation** (*Optional*): Set the rotation of the display. Everything you draw in ``lambda:`` will be rotated
  by this option. One of ``0°`` (default), ``90°``, ``180°``, ``270°``.
- **transform** (*Optional*): Transform the display presentation using hardware. All defaults are ``false``.
   - **mirror_x** (*Optional*, boolean): If true, mirror the physical X axis.
   - **mirror_y** (*Optional*, boolean): If true, mirror the physical Y axis.


**Hardware rotation:** 180 degree rotation can be implemented in hardware as in the following example:

```
    # Example using transform section to achieve 180° rotation using hardware
    display:
      - platform: st7567_spi
        id: my_display
        dc_pin: D3
        transform:
          mirror_x: true
          mirror_y: true
        lambda: |-
          it.print(0, 0, id(my_font), "Hello World!");
```




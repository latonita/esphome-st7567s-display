# GM12864-59N I2C/IIC display (ST7567S) component for ESPHOME
https://github.com/latonita/esphome-st7567s-display/

This is ESPHOME component to support I2C display from Aliexpress/Ebay.
Component idea taken from SPI implementation by @nicolaij (https://github.com/nicolaij/esphomes/tree/main/custom_components/st7567).

Add to your YAML config: 

```
external_components:
  source: github://latonita/esphome-st7567s-display@main
  components: [st7567s]

...

i2c:
  sda: <SDA PIN #>
  scl: <SCK PIN #>
  scan: true

font:
 - file: "gfonts://Roboto"
   id: font_roboto
   size: 24

display:
 - platform: st7567s
   address: 0x3F
   lambda: |-
     it.print(0, 0, id(font_roboto), "Hello World!");

```



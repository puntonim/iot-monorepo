Thursday 2024.11.28 15:38:21

CHRONOTHERMOSTAT PROJECT SPECS


CURRENT THERMOSTAT
==================
Thermostats
-----------
On both floors we use a Perry TE500B to control the heating.
https://www.perry.it/it/prodotto/1tp-te500b-1.aspx
The output is a relay:
   AC 8(2)A/250V
   connected as normally open
Manual: https://docs.perry.it/upload/files/1TITE5xx-1TPTE5xx_DETENN500_07-18.pdf
See manual and photo in this dir.


Caldaia
-------
In the caldaia room we have the same boiler for radiators and hot water.
Modello: Vaillant ecoBLOCK plus VMW IT 306/5-5 R3
Matricola: 21141300100116950001005495N3
Potenza: 30 Kw
Installed on: 2014-08-01
https://targaimpianto.curit.it/infoTarga.jsp?targa=RN7AL64513500003

Electrical power: 80 W -> 80w/220v = 0.36A
Specs: https://www.vaillant.it/downloads/vgoa-vaillant-it-doc/schede-tecniche/murali-condensing-6/scheda-tecnica-ecotec-plus-vmw-2017-982042.pdf


Pumps
-----
Then in our caldaia room there are 2 pumps, one for each floor.
Pump1 power: 0,62 A
Pump2 power: 0,58 A



HW REQUIRED
===========
Relay
-----
- 220V 8A is enough as it is the max for the current thermostat
  And the whole boiler+pumps takes << 5A
- Ask question if I really need a relay in a Arduino Forum
- https://www.amazon.it/GTIWUNG-Fotoaccoppiatore-Channel-Supporta-Trigger/dp/B0DCZ9QRPG/258-7778400-6332300
- Not for 220V: https://store.arduino.cc/products/arduino-4-relays-shield
- Nice schema - but it's not for 220V: https://www.amazon.it/AZDelivery-Modulo-livello-KY-019-Arduino/dp/B07CNR7K9B/258-7778400-6332300

With optional ext supply jumper:
 - https://www.amazon.it/AZDelivery-Modulo-Channel-Optoisolatore-Arduino/dp/B078Q326KT
 - https://www.amazon.it/Heevhas-fotoaccoppiatore-interruttore-alimentazione-azionamento/dp/B0D8Q5C2VG

SRD-05VDC-SL-C
 specs: https://www.circuitbasics.com/wp-content/uploads/2015/11/SRD-05VDC-SL-C-Datasheet.pdf
 info: https://www.circuitbasics.com/setting-up-a-5v-relay-on-the-arduino/
 - https://www.amazon.it/GTIWUNG-Fotoaccoppiatore-Channel-Supporta-Trigger/dp/B0CSJQZ89V/258-7778400-6332300
 - 

Finder 40.31: https://cdn.findernet.com/app/uploads/S40EN.pdf

SPDT Circuits – Single Pole, Double Throw: https://moniteurdevices.com/knowledgebase/knowledgebase/what-is-the-difference-between-spst-spdt-and-dpdt/
Back: https://www.amazon.it/AZDelivery-Channel-Optoaccoppiatore-Arduino-Gratuito/dp/B07LB2RQYP

Questo dice che ci ha fatto un T-stat, e usa anche il bezel:
 https://www.amazon.it/AZDelivery-Modulo-livello-KY-019-Arduino/dp/B07TYG14N6

Guides:
 - spiega come isolare Arduino dallo switch: https://youtu.be/d9evR-K6FAY?si=CB9UNRzN1P_plL2I


Higher quaity:
- https://robot-italy.com/en/products/103020012_grove-spdt-relay-30a-high-current-and-normally-closed
 https://www.reichelt.com/it/it/shop/prodotto/arduino_-_rele_spdt_grove_30_a_sla-05vdc-sl-c-243033
 https://wiki.seeedstudio.com/Grove-SPDT_Relay_30A/
 https://www.mouser.it/ProductDetail/Seeed-Studio/103020012?qs=1%252B9yuXKSi8B%2F5Ke0hFLQzQ%3D%3D
- https://robot-italy.com/en/products/2480-pololu-basic-spdt-relay-carrier-with-5vdc-relay-assembled
- I2C https://robot-italy.com/en/products/com-15093-sparkfun-qwiic-single-relay
- https://robot-italy.com/en/products/dfr0251-gravity-digital-16a-relay-module
 https://wiki.dfrobot.com/16A_Relay_Module_SKU_DFR0251_


Aggiungi foto esistente






PSU for Arduinos
----------------
*Arduino UNO with 220V AC*
It's the main controller.
AC-DC step down?
- Nota il commento in cui suggerisce di mettere fusibili pirma e dopo
 https://www.amazon.it/AZDelivery-%E2%AD%90%E2%AD%90%E2%AD%90%E2%AD%90%E2%AD%90-Adattatore-Arduino-Raspberry/dp/B07C53B1GZ

*Arduino Nano with DC battery*
It's the probe.
Battery Shield?

Rocker Switch 220V
------------------
To switch off from the main power the whole thermostat.
- Switch AC 16A: https://www.amazon.it/OFF-Posizione-SPST-Rocker-Switch/dp/B00WJLF8OQ


Mount bezel
-----------
Search: 16x2 display bezel
Or: case for LCD
https://www.etsy.com/de-en/search?q=2.42inch+LCD+bezel&ref=search_bar
Or I can buy a piece of plexiglass, cut it, glue the display and 
 leds and buttons and then cover it with paint or stickers.
Or the components are fixed in a box and the plexiglass bezel just 
 covers it and it is fixed by screws.
https://www.instructables.com/Home-Made-Bezel-Window-for-LCD-LED-Displays/
Note: I dont need to cut th eplexiglass to show the display, it's transparent!


Better displays
---------------
https://www.amazon.it/Waveshare-Resolution-Communication-Compatible-Raspberry/dp/B0CL6WMHJN


CO2 sensor
----------
Do I want it?
Maybe in the probe?

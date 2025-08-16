Friday 2024.11.29 14:12:10

https://forum.arduino.cc/t/central-heating-choose-relay-and-power-source/1327093

Hello folks!

I am trying to replace the thermostat for the central heating at my parents' house in Italy with Arduino. I am good with software, but I definitely need a hand with electronics!

The project's status is: I’ve got an Arduino with temperature sensors, a display, LEDs, and rotary encoders up and running. But I am missing 2 parts: how to actually switch ON/OFF the central heating with Arduino (a relay?!) and how to power Arduino.

# Existing hardware
![_existing|680x500](upload://rBS3pqxPj1hejRXLPyPrWDKMiDR.png)
Thermostat: [Perry TE500B](https://docs.perry.it/upload/files/1TITE5xx-1TPTE5xx_DETENN500_07-18.pdf)
Gas boiler: [VAILLANT ecoTEC plus VMW 306/5-5](https://www.vaillant.it/downloads/vgoa-vaillant-it-doc/schede-tecniche/murali-condensing-6/scheda-tecnica-ecotec-plus-vmw-2017-982042.pdf)
[Photo](https://ibb.co/4TkKDmB)

# New hardware and design
![_new|690x152](upload://oDGRh4Bxb73Wa17YEm3nZG2vSm9.png)

**Questions on new hardware and design**

* Is a **1-channel relay** the right electrical component?
* If so, is [this](https://www.amazon.it/GTIWUNG-Fotoaccoppiatore-Channel-Supporta-Trigger/dp/B0DCZ9QRPG/258-7778400-6332300) a good choice?
* Would I connect it as in my diagram, so the 2 black wires coming from the boiler to COM and NO (normally open)? Are the 2 black wires interchangeable, so I can connect either to COM or NO?
* And what about the **neutral wire**? Shall I leave it disconnected?
* I searched a bit about relay and [some people](https://youtu.be/d9evR-K6FAY?si=CB9UNRzN1P_plL2I) use an **external 5V power source** to power the relay. This isolates completely Arduino and the load. Is this worth?
* If I want to completely isolate Arduino and the load then I would need a relay that supports an external power source (set with a jumper) and the one I linked above does not. But these two ([first](https://www.amazon.it/AZDelivery-Modulo-Channel-Optoisolatore-Arduino/dp/B078Q326KT), [second](https://www.amazon.it/Heevhas-fotoaccoppiatore-interruttore-alimentazione-azionamento/dp/B0D8Q5C2VG)) seems to support an external supply, right? 
* If the objective in the previous two points is to protect Arduino from the load circuit, then wouldn’t a **fuse** between arduino and the relay be enough?
* Finally: given that currently there are only the 2 black wires (line) and the green neutral, how do I **power Arduino** without using a battery? I guess there is no way, I definitely need to add 2 more wires: live and neutral, right? 

Thank you!
Paolo

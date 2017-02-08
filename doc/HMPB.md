Home-made Power Bank
====================


Many IoT development boards, such as the ESPThing and the NodeMCU are
powered through an USB connector. They are also relatively low power.
So low power in fact, that they won't draw enough power to trip the
minimum current of many commercial USB power bank products. Fortunately,
you can create your very own power bank easily and cheaply from readily
available components.

The shopping list
-----------------

 * One or more 18650 Lithium rechargeable batteries. This will easily
  be the most expensive part of the build. Try and get a cell with
  at least 2000mAh, and beware of scams. If it's advertised as more than
   3500mAh or so, it's probably a fake.

   Could cost anywhere between $2 and $10 depending on brand and reseller.
 * One 18650 battery holder. It holds the battery. Doh.

   Available for $0.40 or so.
 * One 5V boost converter. This is the part that converts the ouput of
    the Lithium cell (nominally 3.7V, but really anywhere between 4.3V
    and 2.5V) to 5V. Get one with a female USB port.

    These things are really really cheap, $0.30 or thereabout.
 * Optionally, get a small charger board that can recharge the battery
   from a micro-USB.

   Since these things can be found inside pretty much any
   rechargeable doohickey, they are also dirt-cheap. Expect to pay about
   $0.30 for such a module that includes a protection IC. If you don't
   get  charger board, you'll need ot recharge the batteries using a
   stand-alone wall charger.


![Shopping list](shopping.png)

In the image above, you can see the items on the list from left to
right, the 18650 battery, the battery holder, the boost converter,
the charger module.

Assembling the power bank
------------------------

Apply a very light coat of solder to the wires of the battery holder.
Thread the wires through the contact pads of the boost converter from
the top. This helps absorb mechanical stresses. Apply solder to the
wires and pads on the bottom. Cut wires if they stick out too much.

Insert a charged 18650. You have a working home-made power bank.

There are two optional steps:

1. Hot glue the boost converter to the back of the battery holder for
 improved portability and resilience.
2. Connect (and optionally glue) the charger module. Connect the
 positive battery terminal (red wire) to the B+ pad on the module.
 Connect the negative battery terminal to the B- pad.















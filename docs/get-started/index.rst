***********
Get Started
***********

The objective of this project is to design a board based on an ESP-WROOM-32 module from the company Espressif and an MPU6050 component from the company Invensense.

The design process is based on 3 steps:
#. Prototyping of the concept from demoboards
#. Software development based on the prototype
#. Board design (schematic and routing).

The Get started section presents the first part of the process, i.e. the prototyping phase.

Introduction
============
The principle of the project is based on the use of two (or more) completely identical boards that communicate using Wifi, on an ad'hoc network (define as ESP_MAD network).

One of the board (named "Server") is initialized in Acces Point (AP) mode. The other boards initialize in Station mode (STA) (named "Client(s)").

The "Server" board and the "Client" board(s) use similar software, but slightly different because of this specificity of Wifi configuration.

The "Server" board integrates a web server, which allows any browser to connect to the "Server" board and to navigate on the developed HTML pages.

The HTML pages are developed using the Bootstrap framework for the layout and JQuery for the Javascript calls embedded in the pages.

To date, the software development only takes into account one client board.

What do you need
================

During the protyping phase, we will use somme breadboards, a development board integrating an ESP-WROOM-32 module, a development board integrating an MPU6050, as well as some prototyping wires.

These components can be easily found on the internet (Bandgood, Alliexpress, etc.), for a few euros.

=================  =================  =================  =================
|Breadboard|       |Lolin32|          |MPU6050|          |Wires|
-----------------  -----------------  -----------------  -----------------
Breadboard         Lolin32 demoboard  MPU6050 demoboard  Prototyping Wires
=================  =================  =================  =================

.. |Breadboard| image:: /_static/breadboard.jpg

.. |Lolin32| image:: /_static/lolin32.jpg

.. |MPU6050| image:: /_static/mpu6050-board.jpg
 
.. |Wires| image:: /_static/prototype-wires.jpg
 
Several demonstration boards based on ESP-WROOM-32 can be used

I personally used the following boards without any problem.

===================  ===================  ==================
|Lolin 32|           |Lolin 32 Lite|      |Wemos X|
-------------------  -------------------  ------------------
Wemos Lolin 32       Wemos Lolin 32 lite  Wemos clone   
===================  ===================  ==================

.. |Lolin 32| image:: /_static/lolin32.jpg

.. |Lolin 32 Lite| image:: /_static/lolin32-lite.jpg

.. |Wemos X| image:: /_static/clone-wemos.jpg


The last board in this table has a Wemos marking, but does not appear to come from the company of the same name. However, it works very well and is a little smaller than the two Lolin 32 Wemos. 

The interest of the two Lolin32 board is however to have a battery connector.

Connection diagram
==================

The ESP32 demoboard is easily connected to the MPU6050 board via I2C.

.. image:: /_static/demoboard-connexion.jpg

.. note:: The I2C pins on the ESP32 demoboard vary depending on the board. If you are using one of the three boards presented above, you will find the pins to use in the `esp_mad.h file <https://github.com/adesandr/GliderThrow_Meter/blob/master/Includes/Esp_mad.h>`_
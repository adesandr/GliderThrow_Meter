.. GliderThrow_Meter documentation master file, created by
   sphinx-quickstart on Sat Mar 28 21:22:55 2020.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to the GliderThrow_Meter's documentation!
=============================================

This is the documentation for the GliderThrow_Meter's project (`GliderThrow_Meter <https://github.com/adesandr/GliderThrow_Meter>`_).

GliderThrow is primary design for setting the control surface of a RC glider but you will find that it can be used on most 
every airplane and for a variety of applications as Measuring a dihedral angle of a wing, Measuring Model Airplane Incidence angle , etc.

GliderThrow meter is made up of two device, each using one `ESP32 <https://www.espressif.com/en/products/hardware/esp32/overview>`_ SOC 
and one `MPU 6050 <https://invensense.tdk.com/products/motion-tracking/6-axis/mpu-6050/>`_ 6 doffs component.

Each device can measure the deflections in degrees / millimeters with a resolution of 0.1 degrees and can measure the differential when 
working together with a second unit since GliderThrow is a system that comprises two sensors, one for each wing or surface control pair of your airplane.

Using a dual system simplifies a lot the throw setting of your model by having a direct view of "equivalent" control surfaces at the same time (left and right aileron, or flap).

As the first device embedded a small http server, the data can be viewed through any web browser on a smartphone (Android, iOS, etc.), PC or MAC.

UI is built using bootstrap and jquery, and all the files needed are embedded in the .rodata segment of the first device.

The project is made up of two parts, the server (Esp_mad_Server directory) and the client (Esp_mad_Client directory).

Two extras libraries are used in the project : i2clibdev and MPU6050 from `jrowberg <https://github.com/jrowberg/i2cdevlib>`_.

These libraries are in the extra_components directrory of the project.

This project is build using the ESP-IDF 4.0 CMake Build System. Please refer to the 
`espressif documentation <https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html>`_ 
for more information to setup an ESP-IDF environnement.

I highly recommande you to use the `Vscode <https://code.visualstudio.com/download>`_ IDE and the 
`expressif ESP-IDF Vscode Extension <https://marketplace.visualstudio.com/items?itemName=espressif.esp-idf-extension>`_ to build the project.

All my thanks to the members of the `Electrolab <https://www.electrolab.fr/>`_ of Nanterre who assisted me during the realization of this project and for the access to the manufacturing and tests tools of this miraculous Hackerspace.

This project is published under the `MIT license <https://github.com/adesandr/GliderThrow_Meter/blob/master/LICENCE>`_.

Enjoy !

==================  ==================  ==================  ==================
|Get Started|_      |Software Design|_  |Hardware Design|_  |System Build|_
------------------  ------------------  ------------------  ------------------
`Get Started`_      `Software Design`_  `Hardware Design`_  `System Build`_
==================  ==================  ==================  ==================

.. |Get_Started| image:: /_static/get-started.png
.. _Get Started: get-started/index.html

.. |Software Design| image:: /_static/software-design.png
.. _Software Design: software-design/index.html

.. |Hardware Design| image:: /_static/hardware-design.png
.. _Hardware Design: hardware-design/index.html

.. |System Build| image:: /_static/system-build.png
.. _System Build: system-build/index.html

.. toctree::
   :hidden:

   Get Started <get-started/index>
   Software Design <software-design/index>
   Hardware Design <hardware-design/index>
   System Build <system-build/index>


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

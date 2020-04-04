.. GliderThrow_Meter documentation master file, created by
   sphinx-quickstart on Sat Mar 28 21:22:55 2020.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to GliderThrow_Meter's documentation!
=============================================

This is the documentation for the GliderThrow_Meter's project (`GliderThrow_Meter <https://github.com/adesandr/GliderThrow_Meter>`_).

GliderThrow is primary design for setting the control surface of a model glider but you will find that it can be used on most 
every airplane and for a variety of applications as Measuring a dihedral angle of a wing, Measuring Model Airplane Incidence angle , etc.

GliderThrow meter is made up of two device, each using one `ESP32 <https://www.espressif.com/en/products/hardware/esp32/overview>`_ SOC 
and one `MPU 6050 <https://invensense.tdk.com/products/motion-tracking/6-axis/mpu-6050/>`_ 6 doffs component.

Each device can measure the deflections in degrees / millimeters with a resolution of 0.1 degrees and can measure the differential when 
working together with a second unit since GliderThrow is a system that comprises two sensors, one for each wing or surface of your airplane.

Using a dual system very much simplifies the throw setting of your model by having a direct view of both control surfaces at the same time.

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

Enjoy !

==================  ==================  ==================
|Get Started|_      |Software Design|_  |API Reference|_
------------------  ------------------  ------------------
`Get Started`_      `Software Design`_  `API Reference`_
==================  ==================  ==================

.. |Get Started| image:: /_static/get-started.png
.. _Get Started: get-started/index.html

.. |Software Design| image:: /_static/software-design.png
.. _Software Design: software-design/index.html

.. |API Reference| image:: /_static/api-reference.png
.. _API Reference: api-reference/index.html

.. toctree::
   :hidden:

   Get Started <get-started/index>
   Sotfware Design <software-design/index>
   API Reference <api-reference/index>


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
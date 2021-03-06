# GliderThrow_Meter

This project proposes a simple digital Angle/Throw meter and ... Differentials, using two esp32 and two MPU 6050 6 DOFFS components.

GliderThrow meter is composed by two device, each using one esp32 and one MPU 6050.

Each device can measure the deflections in degrees / millimeters with a resolution of 0.1 degrees and can measure the differential when working together with a second unit since GliderThrow is a system that comprises two sensors, one for each wing or surface of your airplane.

Using a dual system very much simplifies the throw setting of your model by having a direct view of both control surfaces at the same time.

GliderThrow is primary design for setting the control surface of a RC glider but you will find that it can be used on most every RC airplane and for a variety of applications as Measuring a dihedral angle of a wing, Measuring Model Airplane Incidence angle , etc.

As the first device embedded a small http server, the data can be viewed through any web browser on a smartphone, Android or iOS, PC or MAC.

UI is built using bootstrap and jquery, and all the files needed are embedded in the .rodata segment of the first device.

The project is composed of two parts, the server (Esp_mad_Server directory) and the client (Esp_mad_Client directory).

Two extras libraries are used in the project : i2clibdev and MPU6050 from jrowberg (https://github.com/jrowberg/i2cdevlib).

These libraries are in the extra_components directrory of the project. Take care to update the Makefile given in Esp_mad_Client & Server regarding your own configuration.

For more information, please read the docs at https://gliderthrow-meter.readthedocs.io/en/latest/

Enjoy !

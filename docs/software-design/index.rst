***************
Software Design
***************

Logical design
==============

The following figure shows the overall logical architecture of the system composed of a server and a client.

.. image:: /_static/software-design-architecture.png

Files organization
==================

The project files are organized as follows: 

.. image:: /_static/GliderThrowDirectoryTree.png

 * docs : contains all the documentation related with the project. Documentation of the project is generated using Sphinx (a python documentation generator)
 * Esp-esp_mad_client : contains all the code for the Client part,
 * Esp_mad_Server : contains all the code for the Server part,
 * extra_components : contains the libraries used in the project and two components share between the Server and the Client,
 * Includes : contains Esp_mad.h for the globals define used in the code and Esp_mad_Globals_Variables.h for the declaration of the globals variables used.
 * buildAll.sh : is a little script used to clean or build all the project. The result is stored in the resultBuild.txt.
 * README.md is the presentation of the project used by github and LICENCE is a MIT licence.

The docs directory contains also the bom, the datasheet for the main chip used in the project, the stl files to build the casing and the box, the eagle files and the gerber files.

Server software architecture
============================

The "Server" software code is made up of three files:
 * esp_mad.cpp: this is the launch file which will create three FreeRtos tasks

  * the "measure" task
  * the "http-server" task
  * the "task_vBattery"

 * esp_mad_task_http_server.c: it is the file which contains the code of the task "http-server"
 * esp_mad_task_measure.cpp: it is the file which contains the code of the task "measure".
 * esp_mad_task_vBattery.c : it is the file with contains the code of the task "task_vBattery"

The http-server task
--------------------
A web server is a software component that listens for incoming HTTP requests from web browsers. Upon receiving a request, the web server sends a response. This may be the return of an HTML document to be displayed in a browser or data that forms a response to a service call. An HTTP request can also include data to be sent to the ESP32 for processing. There are many implementations of Web servers that can run in an ESP32 environment.

The Espressif (ESP-IDF) framework provides an API for implementing a lightweight Web server on ESP32.

The HTTP Server component of Espressif (ESP-IDF) allows you to run a lightweight web server on ESP32. The two basic API calls are:

 * httpd_start(): creates an HTTP server instance, allocates resources to it based on the specified configuration, and generates a handle to the server instance. The server will have a listening socket (TCP) for HTTP traffic. The task priority and stack size are configurable when creating the server instance by passing the httpd_config_t structure to httpd_start(). TCP traffic is parsed as HTTP requests and, depending on the requested URI, registered handlers will be called to return HTTP response packets.

 * httpd_stop(): stops the server with the provided handle and releases the associated resources. This is a blocking function that first signals a stop to the server task, and then waits for the task to finish. Upon termination, the task closes all open connections, deletes registered URI handlers, and resets all session context data to empty.

To process HTTP requests sent to the server, you will need to register URI handlers with :

 * httpd_register_uri_handler(): registers a URI handler by passing an httpd_uri_t structure object that has members including the IR name, method type (e.g. HTTPD_GET / HTTPD_POST / HTTPD_PUT etc ...), a function pointer of type esp_err_t * handler (httpd_req_t * req) and user_ctx pointer to the context data.

The http-server task starts by launching a DHCP server, then initializes the board in AP mode by associating the SSID defined in the esp_map.h file.

The address 198.168.1.1 is assigned to the Wifi AP as defined when the DHCP server is launched.

During the initialization of the Wifi in AP mode, an event_group is created to receive the various events that can be received by the Wifi stack.
 * When the *SYSTEM_EVENT_AP_START* event is received, the web server is launched using the httpd function library. When the server is launched, the various URLs on which the server is likely to react are recorded and for each URL a callback function is associated.
 * When the event *SYSTEM_EVENT_AP_STACONNECTED* is received, the corresponding bit is recorded in the event_group.
 * On receipt of the *SYSTEM_EVENT_AP_STADISCONNECTED* event, the corresponding bit is recorded in the event group.
 * On receipt of the *SYSTEM_EVENT_AP_STOP* event, the web server is stopped and the associated resources are released.
 
.. note:: The URL "/", which corresponds to the reception of an HTTP GET at the address 192.168.1.1, i.e. the main page of the site (esp.html), triggers the loading of the elements embedded in the page, which are bootstrap.min.css, bootstrap.min.js and jquery-3.1.1.min.js, by as many HTTP GET requests as required by the client browser. 

All these elements, as well as the esp.html page, are embedded in the .rodata segment of the ESP32 memory (using the EMBED_FILES directive in the project's CMakeList.txt file).

Each element is then referenced in the code using the following two directives :

.. code-block:: c

 extern const uint8_t esp_html_start[] asm("_binary_esp_html_start");
 extern const uint8_t esp_html_end[] asm("_binary_esp_html_end");

.. note:: It is also possible to perform the same operations using a SPI Flash File System (SPIFFS), but I have not tested this solution. For a Web server using more than one HTML page, this method is probably more interesting than the method consisting in embedding the pages in the .rodata segment.

Data calculated by the "Measure" task (angle and travel) are retrieved by the http_server task from memory, these two variables being defined as global variables. These two values are updated by the "Measure" task every 10 ms.

The deflection angle information measured by the "Client" board is received at a frequency of 900 ms by an HTTP POST request. On receipt of the request, the deflection value in mm is calculated according to the control surface chord.

When the chord is changed from a web browser, an HTTP POST request is received and the chordControlSurface global variable is changed.

The "measure" task
------------------

The task "measure" performs the following functions :
 * initialization of the I2C bus,
 * calibration of the MPU6050 component,
 * Then periodically:

  * Reading of the accelerometer and gyroscope values on the axes (x, y, z),
  * Calculation of the angle in degrees based on the previous values.

.. note:: the task "measure" is identical for the "Server" board and the "Client" board. The only difference is that in the case of the "Server" board, the deflection value in mm is calculated periodically by the "measure" task, whereas for the "Client" board, the value of the angle is transmitted to the "Server" board using an HTTP POST request and it is the "Server" board that performs the calculation of the deflection in mm.

Complementary filter is used to combine accelero and gyro datas. see `complementary filter <http://www.pieter-jan.com/node/11>`_ for more information.

Basically complementary filter avoid used of kallman filter, quiet difficult to implement in small platform. Gyro are used for fast motion as accelero are used for slow motion.

.. note:: The deflection value in mm is calculated as a function of the angle by the following formula : X = 2* sin(alpha/2) * L.

.. image:: /_static/formula-angle-travel.png
   :align: center

The "task_vBattery" task
------------------------

The task "vBattery" compute periodically (each 30s per default) the measurement of the voltage of the battery.

The battery voltage is connected to the IO35 pin of the ESP-WROOM-32. This pin is the chanel 7 of the ADC1.

A bridge resistor divider with two resistors of 100 KOhm is used to decreased the voltage from 4.2 V to 2.1 V. So the attenuation of the ADC is set to 11 dB.  

Client software architecture
============================

The "Client" software code is made up of three files:
 * esp_mad_client.cpp: this is the launch file which will create three FreeRtos tasks

  * the "measure" task
  * the "http-client" task
  * the "task_vBattery"

 * esp_mad_task_http_client.c: it is the file which contains the code of the task "http-server"
 * esp_mad_task_measure.cpp: it is the file which contains the code of the task "measure".
 * esp_mad_task_vBattery.c : it is the file which contains the code of the task "task_vBattery".

The measure task
----------------

The measure task is totally the same code than the measure task of the "Server". No more words to add to this section :-)

The esp_map_task_http_client
----------------------------

The "hhtp-client" task start to initialize the board on wifi station.

Then, the task checks periodically if the board is connected to the "Server" Board, and if the MPU6050 calibration is fisnish testing the global Binit variable.

If these conditions are true, an HTTP POST with the angle measure by the board is send to the "Server" board.

The task_vBattery
-----------------

The task_vBattery is totally the same code than the task_vBattery of the "server". No more words to add to this section also :-)

UX Design
=========

The man-machine interface (MMI) of the project consists of a single HTML page (esp.html).

This page is built using the CSS framework `bootstrap <https://getbootstrap.com/>`_.

The page embeds an ajax script which periodically makes a HTTP GET request to the "Server" board which sends back the different information to be displayed in the page. A second script makes it possible to carry out the change of the chord of the control surfaces by a HTTP POST request.

All the files for MMI are located in the directoy GliderThrowMeter/Esp_mad_Server/main/WebsiteFiles

To connect to the page, it is first necessary to connect to the Wifi ad'hoc network of SSID ESP_MAD.

.. image:: /_static/ssid-selection.png
   :align: center

Then, just type the address 192.168.1.1 in the URL bar of your browser to connect to the main page of the project.

.. image:: /_static/menu-travel.png
   :align: center

The main page of the project contains 4 tabs : Travel, Angle, Setting & Info.

The travel tab displayed the current travel of each sensors, and the Maximum up and down for each sensors stored during the operation. The Reset Maximum(s) button is used to set to 0 these Maximum. 

The "Angle" tab selection causes the page showing the deflection angles for both board to be displayed.

.. image:: /_static/menu-angle.png
   :align: center

The "Setting" tab will display the page that allows you to change the value of the control surface chord.

.. image:: /_static/menu-chord.png
   :align: center

.. note:: In the current version, the project allows to control only one "Client" and both boards deal with the same chord value.

To change the value of the chord, modify the value in the input field and validate with "Save change chord" button.

.. image:: /_static/change-chord.png
   :align: center

Finally, the "Info" tab display the voltage of the battery for both sensor.

.. image:: /_static/menu-info.png
   :align: center
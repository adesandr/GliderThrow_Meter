***************
Software Design
***************

Logical design
==============

The following figure shows the overall logical architecture of the system composed of a server and a client.

.. image:: /_static/software-design-architecture.png

Server software architecture
============================

The "Server" software code is made up of three files:
 * esp_mad.cpp: this is the launch file which will create two FreeRtos tasks

  * the "measure" task
  * the "http-server" task

 * esp_mad_task_http_server.c: it is the file which contains the code of the task "http-server"
 * esp_mad_task_measure.cpp: it is the file which contains the code of the task "measure".

The http-server task
--------------------
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

The measure task
----------------
to be complete

Client software architecture
============================
to be complete
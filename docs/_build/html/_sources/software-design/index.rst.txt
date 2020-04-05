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
to be complete

The measure task
----------------
to be complete

Client software architecture
============================
to be complete
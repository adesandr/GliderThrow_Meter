#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

COMPONENT_ADD_INCLUDEDIRS=. WebsiteFiles ..

COMPONENT_EMBED_FILES := WebsiteFiles/esp.html WebsiteFiles/bootstrap.min.css WebsiteFiles/bootstrap.min.js WebsiteFiles/jquery-3.3.1.min.js

COMPONENT_EXTRA_INCLUDES= $(PROJECT_PATH)/GliderThrow_Meter/Includes
idf_component_register(SRCS "esp_mad.cpp" "esp_mad_task_http_server.c"
                       INCLUDE_DIRS "" "${PROJECT_DIR}/../extra_components/MPU6050" "${PROJECT_DIR}/../extra_components/i2clibdev"
                       EMBED_FILES WebsiteFiles/esp.html WebsiteFiles/bootstrap.min.css WebsiteFiles/bootstrap.min.js WebsiteFiles/jquery-3.3.1.min.js)
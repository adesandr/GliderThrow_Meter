idf_component_register(SRCS "esp_mad_task_measure.cpp"
                    INCLUDE_DIRS "" "${PROJECT_DIR}/../Includes" "${PROJECT_DIR}/../extra_components/MPU6050" "${PROJECT_DIR}/../extra_components/i2clibdev"
                    REQUIRES MPU6050)
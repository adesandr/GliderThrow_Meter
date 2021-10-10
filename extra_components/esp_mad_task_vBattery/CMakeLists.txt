idf_component_register(SRCS "esp_mad_task_vBattery.c"
                    INCLUDE_DIRS "" "${PROJECT_DIR}/../Includes" "${IDF_PATH}/components/esp_adc_cal/include"
                    REQUIRES esp_adc_cal)
idf_component_register(SRCS "MPU6050.cpp"
                    INCLUDE_DIRS "" "${PROJECT_DIR}/../extra_components/i2clibdev"
                    REQUIRES i2clibdev)
deps_config := \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\app_trace\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\aws_iot\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\bt\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\driver\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\esp32\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\esp_adc_cal\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\esp_event\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\esp_http_client\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\esp_http_server\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\ethernet\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\fatfs\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\freemodbus\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\freertos\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\heap\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\libsodium\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\log\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\lwip\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\mbedtls\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\mdns\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\mqtt\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\nvs_flash\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\openssl\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\pthread\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\spi_flash\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\spiffs\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\tcpip_adapter\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\unity\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\vfs\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\wear_levelling\Kconfig \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\app_update\Kconfig.projbuild \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\bootloader\Kconfig.projbuild \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\esptool_py\Kconfig.projbuild \
	C:\esp-idf\msys32\home\Eliot\esp\esp-idf\components\partition_table\Kconfig.projbuild \
	/home/Eliot/esp/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)

ifneq "$(IDF_TARGET)" "esp32"
include/config/auto.conf: FORCE
endif
ifneq "$(IDF_CMAKE)" "n"
include/config/auto.conf: FORCE
endif

$(deps_config): ;

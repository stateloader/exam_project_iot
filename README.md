# exam_project_iot
Better documentation in the pipe.


### 'Sensor Device'

![sensor](https://user-images.githubusercontent.com/78800629/214961886-07ba59b5-a249-4bb0-bc67-80acd43ac379.jpg)



### 'Carrier Device'

![carrier](https://user-images.githubusercontent.com/78800629/214961889-4c9483b0-3714-43b1-b33f-391fbc8b9caa.jpg)

###Node configs

Config data regarding the 4 'nodes' at play within the context of "Aktivt Sophantering IoT".
Besides providing a better view over thetopology, it's been quite handy to use for
copy/pasting configs-files while flashing my devices back and forward during tests =)

To create UUID:s I've just used this generator https://www.uuidgenerator.net/version1

-----------------------------------------------------------------------------------------
SENSOR DEVICE   "sensor_device"   (ESP32 Devkit LiPo)             LOCAL ENDPOINT SERVER
-----------------------------------------------------------------------------------------

#define SENSOR_SERVER_NAME   "sensor_device"
#define SENSOR_SERVICE_UUID  "3b54ddce-8aa4-11ed-a1eb-0242ac120002"
#define SENSOR_RESOURCE_UUID "a196c782-8aa4-11ed-a1eb-0242ac120002"

-----------------------------------------------------------------------------------------
CARRIER DEVICE                    (ESP32 Devkit LiPo)                   CARRIER FLOOR 1
-----------------------------------------------------------------------------------------

#define SOURCE_SERVER_NAME   "sensor_device"
#define SOURCE_SERVICE_UUID  "3b54ddce-8aa4-11ed-a1eb-0242ac120002"
#define SOURCE_RESOURCE_UUID "a196c782-8aa4-11ed-a1eb-0242ac120002"

#define DEVICE_CLIENT_NAME   "floor1_client"
#define DEVICE_SERVER_NAME   "floor1_server"
        
#define TARGET_SERVICE_UUID  "8c206542-8d11-11ed-a1eb-0242ac120002"
#define TARGET_RESOURCE_UUID "94cdef66-8d11-11ed-a1eb-0242ac120002"

-----------------------------------------------------------------------------------------
CARRIER DEVICE                    (ESP32 Devkit LiPo)                   CARRIER FLOOR 3
-----------------------------------------------------------------------------------------

#define SOURCE_SERVER_NAME   "floor1_server" MAC 8C:4B:14:82:FE:22
#define SOURCE_SERVICE_UUID  "8c206542-8d11-11ed-a1eb-0242ac120002"
#define SOURCE_RESOURCE_UUID "94cdef66-8d11-11ed-a1eb-0242ac120002"

#define DEVICE_CLIENT_NAME   "floor3_client"
#define DEVICE_SERVER_NAME   "floor3_server"

#define TARGET_SERVICE_UUID  "40f7c1a4-8f97-11ed-a1eb-0242ac120002"
#define TARGET_RESOURCE_UUID "4cd1a9fe-8f97-11ed-a1eb-0242ac120002"

-----------------------------------------------------------------------------------------
CENTRAL DEVICE    "central_jacke"  (RaspberryPi)                  LOCAL ENDPOINT CLIENT
-----------------------------------------------------------------------------------------

#define SOURCE_SERVER_NAME   "floor3_server" MAC 7C:9E:BD:B3:F9:36
#define SOURCE_SERVICE_UUID  "40f7c1a4-8f97-11ed-a1eb-0242ac120002"
#define SOURCE_RESOURCE_UUID "4cd1a9fe-8f97-11ed-a1eb-0242ac120002"

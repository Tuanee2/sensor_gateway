#include "sensor.h"
#include <stdio.h>

int main(){
    Temperature_sensor_t temp_sensor;
    initTemperatureSensor(&temp_sensor, 01, "temp01");
    setTemperature(&temp_sensor, 25.5);
    printf("Temperature: %.2f°C\n", getTemperature(&temp_sensor));
    return 0;
}
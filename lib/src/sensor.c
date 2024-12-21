#include "sensor.h"

void active(sensor_t* sensor){
    sensor->status = ACTIVE;
}

void inactive(sensor_t* sensor){
    sensor->status = INACTIVE;
}

void error(sensor_t* sensor){
    sensor->status = ERROR;
}

sensor_status_t getStatus(sensor_t* sensor){
    return sensor->status;
}

// for temperature sensor

void initTemperatureSensor(Temperature_sensor_t* sensor, int id, const char* name) {
    sensor->sensor.id = id;
    strncpy(sensor->sensor.name, name, sizeof(sensor->sensor.name) - 1);
    sensor->sensor.name[sizeof(sensor->sensor.name) - 1] = '\0'; // Đảm bảo chuỗi kết thúc
    sensor->sensor.status = INACTIVE;
    sensor->temperature = 0.0;
    sensor->threshold = 0.0;
}

void setTemperature(Temperature_sensor_t* sensor, float temp){
    sensor->temperature = temp;
}
float getTemperature(Temperature_sensor_t* sensor){
    return sensor->temperature;
}
void setThreshold4TS(Temperature_sensor_t* sensor, float temp){
    sensor->threshold = temp;
}
float getThreshold4TS(Temperature_sensor_t* sensor){
    return sensor->threshold;
}
int isThresholdExceeded4TS(Temperature_sensor_t* sensor){
    return sensor->temperature > sensor->threshold;
}
void serializeTemperatureSensor(Temperature_sensor_t* sensor, char* buffer) {
    sprintf(buffer, "%d,%.2f", sensor->sensor.id, sensor->temperature);
}
void deserializeTemperatureSensor(const char* buffer, Temperature_sensor_t* sensor) {
    sscanf(buffer, "%d,%f", &sensor->sensor.id, &sensor->temperature);
}


// for humidity sensor

void initHumiditySensor(Humidity_sensor_t* sensor, int id, const char* name) {
    sensor->sensor.id = id;
    strncpy(sensor->sensor.name, name, sizeof(sensor->sensor.name) - 1);
    sensor->sensor.name[sizeof(sensor->sensor.name) - 1] = '\0';
    sensor->sensor.status = INACTIVE;
    sensor->humidity = 0.0;
    sensor->threshold = 0.0;
}

void setHumidity(Humidity_sensor_t* sensor, float hum){
    sensor->humidity = hum;
}
float getHumidity(Humidity_sensor_t* sensor){
    return sensor->humidity;
}
void setThreshold4HS(Humidity_sensor_t* sensor, float hum){
    sensor->threshold = hum;
}
float getThreshold4HS(Humidity_sensor_t* sensor){
    return sensor->threshold;
}
int isThresholdExceeded4HS(Humidity_sensor_t* sensor){
    return sensor->humidity > sensor->threshold;
}

// for pressure sensor

void initPressureSensor(Pressure_sensor_t* sensor, int id, const char* name) {
    sensor->sensor.id = id;
    strncpy(sensor->sensor.name, name, sizeof(sensor->sensor.name) - 1);
    sensor->sensor.name[sizeof(sensor->sensor.name) - 1] = '\0';
    sensor->sensor.status = INACTIVE;
    sensor->pressure = 0.0;
    sensor->threshold = 0.0;
}

void setPressure(Pressure_sensor_t* sensor, float pres){
    sensor->pressure = pres;
}
float getPressure(Pressure_sensor_t* sensor){
    return sensor->pressure;
}
void setThreshold4PS(Pressure_sensor_t* sensor, float pres){
    sensor->threshold = pres;
}
float getThreshold4PS(Pressure_sensor_t* sensor){
    return sensor->threshold;
}
int isThresholdExceeded4PS(Pressure_sensor_t* sensor){
    return sensor->pressure > sensor->threshold;
}
#ifndef SENSOR_H
#define SENSOR_H

#include <string.h>

typedef enum{
    ACTIVE,
    INACTIVE,
    ERROR
}sensor_status_t;


typedef struct {
    int id;
    char name[30];
    sensor_status_t status;
}sensor_t;

void active(sensor_t* sensor);
void inactive(sensor_t* sensor);
void error(sensor_t* sensor);
sensor_status_t getStatus(sensor_t* sensor);

typedef struct {
    sensor_t sensor;

    float temperature;

    float threshold;

}Temperature_sensor_t;

void initTemperatureSensor(Temperature_sensor_t* sensor, int id, const char* name);
void setTemperature(Temperature_sensor_t* sensor, float temp);
float getTemperature(Temperature_sensor_t* sensor);
void setThreshold4TS(Temperature_sensor_t* sensor, float temp);
float getThreshold4TS(Temperature_sensor_t* sensor);
int isThresholdExceeded4TS(Temperature_sensor_t* sensor);

typedef struct {
    sensor_t sensor;
    float humidity;
    float threshold;
}Humidity_sensor_t;

void initHumiditySensor(Humidity_sensor_t* sensor, int id, const char* name);
void setHumidity(Humidity_sensor_t* sensor, float hum);
float getHumidity(Humidity_sensor_t* sensor);
void setThreshold4HS(Humidity_sensor_t* sensor, float hum);
float getThreshold4HS(Humidity_sensor_t* sensor);
int isThresholdExceeded4HS(Humidity_sensor_t* sensor);

typedef struct {
    sensor_t sensor;
    float pressure;
    float threshold;
}Pressure_sensor_t;

void initPressureSensor(Pressure_sensor_t* sensor, int id, const char* name);
void setPressure(Pressure_sensor_t* sensor, float pres);
float getPressure(Pressure_sensor_t* sensor);
void setThreshold4PS(Pressure_sensor_t* sensor, float pres);
float getThreshold4PS(Pressure_sensor_t* sensor);
int isThresholdExceeded4PS(Pressure_sensor_t* sensor);

#endif
#include <stdio.h>
#include <string.h>
#include "structs.h"
#include <stdlib.h>
#define SIZE 50000

void *init_info_senzor(int sensor_type, FILE *read)
{
	float voltage = 0, current = 0, power_consumption = 0;
	int energy_regen = 0, energy_storage = 0;

	float pressure = 0, temperature = 0;
	int wear_level = 0, performance_score = 0;

	void *idx = malloc(SIZE * sizeof(char));
	if (!idx)
		return 0;

	if (sensor_type == 1) //  senzor de tip PMU
	{
		fread(&voltage, sizeof(float), 1, read);
		fread(&current, sizeof(float), 1, read);
		fread(&power_consumption, sizeof(float), 1, read);
		fread(&energy_regen, sizeof(int), 1, read);
		fread(&energy_storage, sizeof(int), 1, read);

		*(float *)idx = voltage;
		idx += sizeof(float);

		*(float *)idx = current;
		idx += sizeof(float);

		*(float *)idx = power_consumption;
		idx += sizeof(float);

		*(int *)idx = energy_regen;
		idx += sizeof(int);

		*(int *)idx = energy_storage;
		idx += sizeof(int);
	}
	else //  senzor de tip Tire
	{
		fread(&pressure, sizeof(float), 1, read);
		fread(&temperature, sizeof(float), 1, read);
		fread(&wear_level, sizeof(int), 1, read);
		fread(&performance_score, sizeof(int), 1, read);

		*(float *)idx = pressure;
		idx += sizeof(float);

		*(float *)idx = temperature;
		idx += sizeof(float);

		*(int *)idx = wear_level;
		idx += sizeof(int);

		*(int *)idx = performance_score;
		idx += sizeof(int);
	}
	return idx;
}
void print(void *trimite, int sensor_type)
{
	float voltage = 0, current = 0, power_consumption = 0;
	int energy_regen = 0, energy_storage = 0;

	float pressure = 0, temperature = 0;
	int wear_level = 0, performance_score = 0;

	if (sensor_type == 1) //  senzor de tip PMU
	{

		voltage = *(float *)trimite;
		trimite += sizeof(float);

		current = *(float *)trimite;
		trimite += sizeof(float);

		power_consumption = *(float *)trimite;
		trimite += sizeof(float);

		energy_regen = *(int *)trimite;
		trimite += sizeof(int);

		energy_storage = *(int *)trimite;
		trimite += sizeof(int);

		printf("Power Management Unit\n");
		printf("Voltage:%.2f\n", voltage);
		printf("Current:%.2f\n", current);
		printf("Power Consumption:%.2f\n", power_consumption);
		printf("Energy Regen:%d%%\n", energy_regen);
		printf("Energy Storage:%d%%\n", energy_storage);
	}
	else  //  senzor de tip Tire
	{
		pressure = *(float *)trimite;
		trimite += sizeof(float);

		temperature = *(float *)trimite;
		trimite += sizeof(float);

		wear_level = *(int *)trimite;
		trimite += sizeof(int);

		performance_score = *(int *)trimite;
		trimite += sizeof(int);

		printf("Tire Sensor\n");
		printf("Pressure:%.2f\n", pressure);
		printf("Temperature:%.2f\n", temperature);
		printf("Wear Level:%d%%\n", wear_level);
		if(performance_score == 0)
			printf("Not Calculated!");
		else
			printf("Performance Score:%d%%\n", performance_score);
	}
}
void analyze(void *trimite, int operator, void **operations)
{
	operations = (void **)malloc(sizeof(void*)*8);
	if(!operations)
		return;
	for (int i = 0; i < 8 ; i++) {
		operations[i] = (void*)malloc(static void *sizeof(void));
		if(!operations[i])
			return;
	}
	get_operations(operations);
	operations[operator](trimite);
	if(operator == 0) {
		tire_pressure_status(trimite);
		return;
	}
	if(operator == 1) {
		tire_temperature_status(trimite);
		return;
	}
	if(operator == 2) {
		tire_wear_level_status(trimite);
		return;
	}
	if(operator == 3) {
		tire_performance_score(trimite);
		return;
	}
	if(operator == 4) {
		pmu_compute_power(trimite);
		return;
	}
	if(operator == 5) {
		pmu_regenerate_energy(trimite);
		return;
	}
	if(operator == 6) {
		pmu_get_energy_usage(trimite);
		return;
	}
	if(operator == 7) {
		pmu_is_battery_healthy(trimite);
		return;
	}
}
int check(void *trimite, int sensor_type)
{
	float voltage = 0, current = 0, power_consumption = 0;
	int energy_regen = 0, energy_storage = 0;

	float pressure = 0, temperature = 0;
	int wear_level = 0;

	if (sensor_type == 1) //  senzor de tip PMU
	{

		voltage = *(float *)trimite;
		if(voltage >= 10 && voltage <= 20)
			return 0;
		trimite += sizeof(float);

		current = *(float *)trimite;
		if(current >= -100 && current <= 100)
			return 0;
		trimite += sizeof(float);

		power_consumption = *(float *)trimite;
		if (power_consumption >= 0 && power_consumption <= 1000)
			return 0;
		trimite += sizeof(float);

		energy_regen = *(int *)trimite;
		if (energy_regen >= 0 && energy_regen <= 100)
			return 0;
		trimite += sizeof(int);

		energy_storage = *(int *)trimite;
		if (energy_storage >= 0 && energy_storage <= 100)
			return 0;
		trimite += sizeof(int);
	}
	else  //  senzor de tip Tire
	{
		pressure = *(float *)trimite;
		if (pressure >= 19 && pressure <= 28)
			return 0;
		trimite += sizeof(float);

		temperature = *(float *)trimite;
		if (temperature >= 0 && temperature <= 120)
			return 0; //  senzor eronat
		trimite += sizeof(float);

		wear_level = *(int *)trimite;
		if(wear_level >= 0 && wear_level <= 100)
			return 0;
		trimite += sizeof(int);

		//performance_score = *(int *)trimite;
		//trimite += sizeof(int);

	}
	return 1; //senzorul nu e eronat
}
int main(int argc, char const *argv[])
{
	int nr_senzori = 0, sensor_type = 0, nr_operations = 0, operation = 0;
	int index = 0;
	char string[]="";
	
	FILE *read = fopen(argv[1], "rb");  //  argv[0] = executabilul, argv[1] = fisierul binar
	if (!read) //  citirea nu s-a realizat cu succes
		return 0;

	fread(&nr_senzori, sizeof(int), 1, read);

	sensor *Senzori = malloc(nr_senzori * sizeof(sensor));
	if (!Senzori)
		return 0;
	int k = 0, k1 = 0;
	sensor *Senzori1 = malloc(nr_senzori * sizeof(sensor));
	if(!Senzori1)
		return 0;
	
	for(int i = 0; i < nr_senzori; i++) {
		fread(&sensor_type, sizeof(int), 1, read);
		void *info = init_info_senzor(sensor_type, read);
		//  adaug toate campurile in vectorul Senzori pentru elementul de pe poz k de tip PMU
		if(sensor_type == 1) {
			Senzori[k].sensor_data = info;
			Senzori[k].sensor_type = PMU;
			fread(&nr_operations, sizeof(int), 1, read);
			Senzori[k].nr_operations = nr_operations;
			for (int j = 0; j < nr_operations; j++){
				fread(&operation, sizeof(int), 1, read);
				Senzori[i].operations_idxs[j] = operation;
			}
			k++;
		//  adaug toate campurile in vectorul Senzori1 pentru elem de pe poz k1 de tip TIRE
		} else {
			Senzori1[k1].sensor_data = info;
			Senzori1[k1].sensor_type = TIRE;
			fread(&nr_operations, sizeof(int), 1, read);
			Senzori1[k1].nr_operations = nr_operations;
			for (int j = 0; j < nr_operations; j++){
				fread(&operation, sizeof(int), 1, read);
				Senzori1[i].operations_idxs[j] = operation;
			}
			k++;
		}
	}
	//  concatenez vectorul Senzori1 la vectorul Senzori
	//  dupa ce termin de parcurs fisierul binar, nu ma mai pot intoarce/ citi ceva din el
	//  vector = pointer
	for (int i = 0; i < k1; i++) {
		Senzori[k].sensor_data = Senzori1[i].sensor_data;
		Senzori[k].sensor_type = Senzori1[i].sensor_type;
		Senzori[k].nr_operations = Senzori1[i].nr_operations;
		for (int j = 0; j < k1; j++)
			Senzori[k].operations_idxs[j] = Senzori1[i].operations_idxs[j];
		k++;
	}
	scanf("%s", string);  // e deja adresa string (pointer) -> sir de caractere
	while (strcmp (string, "exit") != 0) {
		if(strcmp (string, "print") == 0) {
			scanf("%d", &index);
			if (index < 0 || index >= k)
				printf("Index not in range!\n");
			else {
				if(Senzori[index].sensor_type == PMU)  //  PMU == 1 (sensor_type)
					print(Senzori[index].sensor_data, 1);
				else
					print(Senzori[index].sensor_data, 0);  //  TIRE == 0 (sensor_type)
			}
		} else {
			if (strcmp (string, "analyze") == 0) {
				scanf("%d", &index);
				if(index < 0 || index >= k)
					printf("Index not in range!\n");
				else {
					for (int j = 0; j < Senzori[index].nr_operations; j++)
						analyze(Senzori[index].sensor_data, Senzori[index].operations_idxs[j]);
					}
			} else {
				if (strcmp(string, "clear") == 0) {
					int m = 0; // pentru refacerea vetorului incepand de la pozitia 0
					for (int i = 0; i < nr_senzori; i++) {
						int ok = check(Senzori[i].sensor_data, Senzori[i].sensor_type);
						if (ok == 1) {
							Senzori[m].sensor_data = Senzori[i].sensor_data;
							Senzori[m].sensor_type = Senzori[i].sensor_type;
							Senzori[m].nr_operations = Senzori[i].nr_operations;
							for(int j = 0; j < Senzori[i].nr_operations; j++)
								Senzori[m].operations_idxs[j] = Senzori[i].operations_idxs[j];
							}
							print(Senzori[m].sensor_data, Senzori[m].sensor_type);
							m++;
						}
						nr_senzori = m;
					}
				}
			}
		scanf("%s", string);
	}
	if (strcmp (string, "exit") == 0) {
		free(Senzori);
		free(Senzori1);
	}
	return 0;
}

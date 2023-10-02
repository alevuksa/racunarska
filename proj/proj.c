#include "stdlib.h"
#include "stdio.h"
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <time.h>

/*
 * Kompajlira se sa: gcc proj.c -o proj -lwiringPi -lwiringPiDev
 */

#define	LED0	0
#define	LED1	1
#define	LED2	2
#define	LED3	3

#define X_HIGH	230
#define X_LOW	50
#define Y_HIGH	230
#define Y_LOW	50

#define MAX_LEVEL	10

int level[10];
int user_level[10];

int display_level(int lvl)
{
	for(int i = 0; i < lvl; i++)
	{
		digitalWrite(level[i], 1);
		delay(1000);
		digitalWrite(level[i], 0);
		delay(200);
	}
};

int level_rand(int lvl)
{
	for(int i = 0; i < lvl; i++)
	{
		level[i] = rand() % 4;
	}
}


int main()
{
	int fd;
	wiringPiSetup();
	fd = wiringPiI2CSetup(0x48);
	time_t t;
	srand((unsigned) time(&t));
	
	pinMode(LED0, OUTPUT);
	pinMode(LED1, OUTPUT);
	pinMode(LED2, OUTPUT);
	pinMode(LED3, OUTPUT);
	
	digitalWrite(LED0, 0);
	digitalWrite(LED1, 0);
	digitalWrite(LED2, 0);
	digitalWrite(LED3, 0);

	delay(100);

	digitalWrite(LED0, 1);
	digitalWrite(LED1, 1);
	digitalWrite(LED2, 1);
	digitalWrite(LED3, 1);

	delay(500);	
	
	digitalWrite(LED0, 0);
	digitalWrite(LED1, 0);
	digitalWrite(LED2, 0);
	digitalWrite(LED3, 0);

	int x_value, y_value;
	unsigned char igra = 1;
	unsigned char nivo = 1;
	int lvl = 1;
	int n = 0;

	while(1)
	{
		while(igra == 1)
		{
			printf("POCINJE NIVO %d\n\n", lvl);
			n = 0;
			nivo = 1;
			level_rand(lvl);
			display_level(lvl);
			
			while(nivo == 1)
			{
				x_value = wiringPiI2CReadReg8(fd, 0x48);
				y_value = wiringPiI2CReadReg8(fd, 0x48 + 1);

				if(x_value > X_HIGH)	
				{
					digitalWrite(LED2, 1);
					delay(1500);
					digitalWrite(LED2, 0);
					user_level[n] = 2;
					n++;
				}

				if(x_value < X_LOW)
				{
					digitalWrite(LED3, 1);
					delay(1500);
					digitalWrite(LED3, 0);
					user_level[n] = 3;
					n++;
				}
				
				if(y_value > Y_HIGH)	
				{
					digitalWrite(LED1, 1);
					delay(1500);
					digitalWrite(LED1, 0);
					user_level[n] = 1;
					n++;
				}

				if(y_value < Y_LOW)
				{
					digitalWrite(LED0, 1);
					delay(1500);
					digitalWrite(LED0, 0);
					user_level[n] = 0;
					n++;
				}
			
				if(n == lvl) nivo = 0;	
			}
			digitalWrite(LED0, 0);
			digitalWrite(LED1, 0);
			digitalWrite(LED2, 0);
			digitalWrite(LED3, 0);
			printf("GOTOV UNOS\n");	
			for(int i = 0; i < lvl; i++)
			{
				if(level[i] != user_level[i])	igra = 0;
				printf("%d\n", user_level[i]);
			}

			lvl++;
			if(igra == 1 && lvl == 11) 
			{
				igra = 0;
				printf("POBEDILI STE!\n");
			}
			else if(igra == 1)
			{
				printf("PRESLI STE NIVO %d\n", lvl-1);
				delay(500);
				printf("SLEDECI NIVO POCINJE ZA 3...");
				delay(1000);
				printf(" 2...");
				delay(1000);
				printf(" 1...");
				delay(1000);
			}
			else
			{	
				printf("POGRESILI STE NIVO\n");
			}
		}

		printf("KRAJ IGRE\n");
		return 0;

		// Kraj igre	
	}
	return 0;
}

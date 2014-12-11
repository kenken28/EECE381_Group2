/*
 * sd_card_controller.c
 *
 *  Created on: 2013-10-08
 *      Author: EECE381 Group 11
 */

#include "sd_card_controller.h"
#include "project.h"

alt_up_sd_card_dev* sd_card_reference;

/*
 * Initialization for SD card controller
 */
void initializeSDCardController()
{
	sd_card_reference = alt_up_sd_card_open_dev("/dev/sd_card");
}

/*
 * Detect and connect to SD Card
 */
void connectToSDCard()
{
	if (sd_card_reference != 0)
	{
		// repeated check the connectivity of the SD CARD
		while (alt_up_sd_card_is_Present() == false);
		while (alt_up_sd_card_is_Present())
		{
			printf("Card connected.\n");
			if (alt_up_sd_card_is_FAT16())
			{
				printf("FAT16 file system detected.\n");
				break;
			}
			else
			{
				printf("Unknown file system.\n");
			}
		}
	}
}




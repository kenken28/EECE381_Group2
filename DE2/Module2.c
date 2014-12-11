#include "usb/usb.h"
#include "altera_up_avalon_usb.h"
#include "system.h"
#include "sys/alt_timestamp.h"
#include <assert.h>

#include "project.h"
#include "sound_manager.h"
#include "sd_card_controller.h"

#define leds (char*) 0x000048a0
#define drawer_base (volatile int *) 0x4840
#define switches (volatile char *) 0x48b0

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define NUM_PIXEL 320*240




unsigned int pic_rec[240*320];

#include "altera_up_avalon_video_pixel_buffer_dma.h"

void drawPic(){
	int x,y;
	for(y=0; y < 240 ; y++) {
								for(x=0; x < 320 ; x++){
									IOWR_32DIRECT(drawer_base,0,x); // Set x1
									IOWR_32DIRECT(drawer_base,4,y); // Set y1
									IOWR_32DIRECT(drawer_base,8,x); // Set x2
									IOWR_32DIRECT(drawer_base,12,y); // Set y2
									IOWR_32DIRECT(drawer_base,16,pic_rec[y*SCREEN_WIDTH+x]);  // Set colour
									IOWR_32DIRECT(drawer_base,20,1);  // Start drawing
									while(IORD_32DIRECT(drawer_base,20)==0); // wait until done
								}
							}
}


int main() {

	int switchFlag = 0;
	int i;
	int bytes_expected;
	int bytes_recvd;
	int total_recvd;
	unsigned char data[4] = {0,0,0,0};
	unsigned char message_tx[] = "EECE381 is so much fun";
	unsigned char message_rx[240*320*2+100];

	printf("USB Initialization\n");
	alt_up_usb_dev * usb_dev;
	usb_dev = alt_up_usb_open_dev(USB_0_NAME);
	assert(usb_dev);
	usb_device_init(usb_dev, USB_0_IRQ);
	printf("Polling USB device. Run middleman now!\n");

	alt_timestamp_start();
	int clocks = 0;
	while (clocks < 50000000 * 10) {
		clocks = alt_timestamp();
		usb_device_poll();
	}
	printf("Done polling USB\n");
	printf("Sending the message to the Middleman\n");

	// Start with the number of bytes in our message
	unsigned char message_length = strlen(message_tx);
	usb_device_send(&message_length, 1);
	// Now send the actual message to the Middleman
	usb_device_send(message_tx, message_length);

	int x,y;


	// Use the name of your pixel buffer DMA core
	alt_up_pixel_buffer_dma_dev *pixel_buffer;

	pixel_buffer = alt_up_pixel_buffer_dma_open_dev(BUFFER_DMA_NAME);

	alt_up_pixel_buffer_dma_change_back_buffer_address(pixel_buffer,PIXEL_BUFFER_BASE);
	alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
	while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer))
			;

	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 0);

	int p,q;
	for(p=0; p<SCREEN_HEIGHT; p++){
		for(q=0; q<SCREEN_WIDTH; q++)
			pic_rec[p*SCREEN_WIDTH+q] = 0x00;
	}


    //Initialize audio and SD card
	initializeSDCardController();
	connectToSDCard();
	//initialize_audio();



	while (1) {

		// Now receive the message from the Middleman
		printf("Waiting for data to come back from the Middleman\n");
		// First 4 bytes are the number of characters in our message
		bytes_expected = 4;
		total_recvd = 0;

		//int pos=0;
		while (total_recvd < bytes_expected) {
			bytes_recvd = usb_device_recv(&data, 4);
			if (bytes_recvd > 0)
				total_recvd += bytes_recvd;

			if (*switches == 0x01){

						IOWR_32DIRECT(drawer_base,0,0); // Set x1
						IOWR_32DIRECT(drawer_base,4,0); // Set y1
						IOWR_32DIRECT(drawer_base,8,319); // Set x2
						IOWR_32DIRECT(drawer_base,12,239); // Set y2
						IOWR_32DIRECT(drawer_base,16,0x0000);  // Set colour
						IOWR_32DIRECT(drawer_base,20,1);  // Start drawing
						while(IORD_32DIRECT(drawer_base,20)==0); // wait until done


				//printf("done clearing screen\n");
			}else if(*switches == 0x02){
				loadFromSDCard();
				drawPic();
			}else if(*switches == 0x00){
				switchFlag = 0;
			}

		}
		unsigned int num_to_receive = data[0] << 24 | (data[1] & 0xFF) << 16 | (data[2] & 0xFF) << 8 | (data[3] & 0xFF);

		//int num_to_receive = (int) data;
		printf("len = %x %x %x %x \n", data[0], data[1], data[2], data[3]);
		printf("About to receive %d bytes:\n", num_to_receive);
		bytes_expected = num_to_receive;
		total_recvd = 0;

		while (total_recvd < bytes_expected) {
			bytes_recvd = usb_device_recv(message_rx + total_recvd, 1);
			if (bytes_recvd > 0)
				total_recvd += bytes_recvd;
		}

/*		int k;
		for(k=0; k<num_to_receive; k++)
			printf("0x%x ", message_rx[k]);
*/

		int k;
		for(k=0,i=0; i<num_to_receive; i=i+2,k++){
			pic_rec[k] = 0x00 << 24 | (0x00 & 0xFF) << 16 | (message_rx[i] & 0xFF) << 8 | (message_rx[i+1] & 0xFF);
		//	printf("0x%x ", pic_rec[k]);
		}

		/*
		for(y=0; y < SCREEN_HEIGHT ; y++) {
				for(x=0; x < SCREEN_WIDTH ; x++){

					alt_up_pixel_buffer_dma_draw(pixel_buffer, pic_rec[y*SCREEN_WIDTH+x], x,  y);
					//usleep(100);
					//printf("0x%x ", pic_rec[y*SCREEN_WIDTH+x]);
				}
			}
		*/

		drawPic();


		saveToSDCard();

/*
		int xxx = atoi(message_rx);
		switch (xxx) {
		case 0:
			*leds = 0x01;
			break;
		case 1:
			*leds = 0x02;
			break;
		case 2:
			*leds = 0x04;
			break;
		case 3:
			*leds = 0x08;
			break;
		case 4:
			*leds = 0x10;
			break;
		case 5:
			*leds = 0x20;
			break;
		case 6:
			*leds = 0x40;
			break;
		case 7:
			*leds = 0x80;
			break;
		default:
			break;
		}*/

	//	printf("\n");
	//	printf("Message Echo Complete\n");
	}
	return 0;
}


void loadFromSDCard(){

	short int file_handle = alt_up_sd_card_fopen("save.dat", false);
	if(file_handle<0){
		printf("Fail to load data!");
		return;
	}

	int i;

	// skip header

	char d1,d2;
	for (i = 0; i < NUM_PIXEL; i++)
	{
		d1 = alt_up_sd_card_read(file_handle);
		d2 = alt_up_sd_card_read(file_handle);
		pic_rec[i] = 0x0000ffff & ((d2 & 0x000000FF) << 8 | (d1 & 0x000000FF));
		//printf("0x%x ",pic_rec[i]);
	}
	//printf("\n");

	alt_up_sd_card_fclose(file_handle);
}


void saveToSDCard(){
	short int file_handle = alt_up_sd_card_fopen("save.dat", false);

	if(file_handle<0){
		printf("Fail to save data!");
		return;
	}

	int i;
	unsigned int iTemp;
	for(i=0; i<NUM_PIXEL; i++){
		iTemp = pic_rec[i];
		alt_up_sd_card_write(file_handle, (char)(iTemp & 0x000000ff));
		alt_up_sd_card_write(file_handle, (char)((iTemp>>8) & 0x000000ff));
		//printf("0x%x ",pic_rec[i]);
	}
	//printf("\n");

	alt_up_sd_card_fclose(file_handle);
}






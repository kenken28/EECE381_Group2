 /*
 * sound_manager.c
 *
 *  Created on: 2013-10-08
 *      Author: EECE381 Group 11
 */

#include "sound_manager.h"
#include "sd_card_controller.h"
#include "project.h"


alt_up_audio_dev * audio_dev;

unsigned int* begin_data;
unsigned int* victory_data;
unsigned int* lose_data;
unsigned int* background_data;
unsigned int* bullet_data;
unsigned int* hit_data;

unsigned int* sound_buff;	//pointer to sound data for writing to FIFO

unsigned int byte_data[2];
unsigned int sound_data_counter = 0;

void av_config_setup()
{
	alt_up_av_config_dev * av_config = alt_up_av_config_open_dev("/dev/audio_and_video_config");
	while (!alt_up_av_config_read_ready(av_config)) {}
}

void initialize_audio()
{
	//initial setup for AV
	av_config_setup();

	// open the Audio port
	audio_dev = alt_up_audio_open_dev("/dev/audio");
	if ( audio_dev == 0)
		printf ("Error: could not open audio device \n");
	else
		printf ("Opened audio device \n");

	//connect to SD card
	connectToSDCard();
	//load sound data into our device
	load_sound_data();
	//initialize audio interrupt
	initialize_audio_irq();
}


//initialize irq for audio write
void initialize_audio_irq()
{
	printf("Initializing Audio IRQ...\n");

	alt_up_audio_disable_read_interrupt(audio_dev);
	alt_up_audio_disable_write_interrupt(audio_dev);

	printf("Audio IRQ initialized...\n");
}

static void begin_isr_0(void* context, alt_u32 id)
{
	// FIFO is 75% empty, need to fill it up
	int sample_counter;
	for (sample_counter = 0; sample_counter < SAMPLE_SIZE; sample_counter++)
	{
		// take 2 bytes at a time
		byte_data[0] = begin_data[sound_data_counter];
		byte_data[1] = begin_data[sound_data_counter+1];
		sound_data_counter += 2;

		// combine the two bytes and store into sample buffer
		sound_buff[sample_counter] = (byte_data[1] << 8) | byte_data[0];

		// if we finish reading our data buffer, then we loop back to start over
		if (sound_data_counter >= BEGIN_SIZE)
		{
			sound_data_counter = 0;
			alt_up_audio_disable_write_interrupt(audio_dev);
			free(begin_data);
			begin_data = 0;//NULL;
		}
	}
	// finally, we write this sample data to the FIFO
	alt_up_audio_write_fifo(audio_dev, sound_buff, SAMPLE_SIZE, ALT_UP_AUDIO_LEFT);
	alt_up_audio_write_fifo(audio_dev, sound_buff, SAMPLE_SIZE, ALT_UP_AUDIO_RIGHT);
}

static void lose_isr_0(void* context, alt_u32 id)
{
	// FIFO is 75% empty, need to fill it up
	int sample_counter;
	for (sample_counter = 0; sample_counter < SAMPLE_SIZE; sample_counter++)
	{
		// take 2 bytes at a time
		byte_data[0] = lose_data[sound_data_counter];
		byte_data[1] = lose_data[sound_data_counter+1];
		sound_data_counter += 2;

		// combine the two bytes and store into sample buffer
		sound_buff[sample_counter] = (byte_data[1] << 8) | byte_data[0];

		// if we finish reading our data buffer, then we loop back to start over
		if (sound_data_counter >= LOSE_SIZE)
		{
			sound_data_counter = 0;
			alt_up_audio_disable_write_interrupt(audio_dev);
		}
	}
	// finally, we write this sample data to the FIFO
	alt_up_audio_write_fifo(audio_dev, sound_buff, SAMPLE_SIZE, ALT_UP_AUDIO_LEFT);
	alt_up_audio_write_fifo(audio_dev, sound_buff, SAMPLE_SIZE, ALT_UP_AUDIO_RIGHT);
}

static void background_isr_0(void* context, alt_u32 id)
{
	// FIFO is 75% empty, need to fill it up
	int sample_counter;
	for (sample_counter = 0; sample_counter < SAMPLE_SIZE; sample_counter++)
	{
		// take 2 bytes at a time
		byte_data[0] = background_data[sound_data_counter];
		byte_data[1] = background_data[sound_data_counter+1];
		sound_data_counter += 2;

		// combine the two bytes and store into sample buffer
		sound_buff[sample_counter] = (byte_data[1] << 8) | byte_data[0];

		// if we finish reading our data buffer, then we loop back to start over
		if (sound_data_counter >= BACKGROUND_SIZE)
		{
			sound_data_counter = 0;
		}
	}
	// finally, we write this sample data to the FIFO
	alt_up_audio_write_fifo(audio_dev, sound_buff, SAMPLE_SIZE, ALT_UP_AUDIO_LEFT);
	alt_up_audio_write_fifo(audio_dev, sound_buff, SAMPLE_SIZE, ALT_UP_AUDIO_RIGHT);
}

static void victory_isr_0(void* context, alt_u32 id)
{
	// FIFO is 75% empty, need to fill it up
	int sample_counter;
	for (sample_counter = 0; sample_counter < SAMPLE_SIZE; sample_counter++)
	{
		// take 2 bytes at a time
		byte_data[0] = victory_data[sound_data_counter];
		byte_data[1] = victory_data[sound_data_counter+1];
		sound_data_counter += 2;

		// combine the two bytes and store into sample buffer
		sound_buff[sample_counter] = (byte_data[1] << 8) | byte_data[0];

		// if we finish reading our data buffer, then we loop back to start over
		if (sound_data_counter >= VICTORY_SIZE)
		{
			sound_data_counter = 0;
			alt_up_audio_disable_write_interrupt(audio_dev);
		}
	}
	// finally, we write this sample data to the FIFO
	alt_up_audio_write_fifo(audio_dev, sound_buff, SAMPLE_SIZE, ALT_UP_AUDIO_LEFT);
	alt_up_audio_write_fifo(audio_dev, sound_buff, SAMPLE_SIZE, ALT_UP_AUDIO_RIGHT);
}


static void bullet_isr_0(void* context, alt_u32 id)
{
	// FIFO is 75% empty, need to fill it up
	int sample_counter;
	for (sample_counter = 0; sample_counter < SAMPLE_SIZE; sample_counter++)
	{
		// take 2 bytes at a time
		byte_data[0] = bullet_data[sound_data_counter];
		byte_data[1] = bullet_data[sound_data_counter+1];
		sound_data_counter += 2;

		// combine the two bytes and store into sample buffer
		sound_buff[sample_counter] = (byte_data[1] << 8) | byte_data[0];

		// if we finish reading our data buffer, then we loop back to start over
		if (sound_data_counter >= BULLET_SIZE)
		{
			sound_data_counter = 0;
			alt_up_audio_disable_write_interrupt(audio_dev);
		}
	}
}


static void hit_isr_0(void* context, alt_u32 id)
{
		// FIFO is 75% empty, need to fill it up
		int sample_counter;
		for (sample_counter = 0; sample_counter < SAMPLE_SIZE; sample_counter++)
		{
			// take 2 bytes at a time
			byte_data[0] = hit_data[sound_data_counter];
			byte_data[1] = hit_data[sound_data_counter+1];
			sound_data_counter += 2;

			// combine the two bytes and store into sample buffer
			sound_buff[sample_counter] = (byte_data[1] << 8) | byte_data[0];

			// if we finish reading our data buffer, then we loop back to start over
			if (sound_data_counter >= HIT_SIZE)
			{
				sound_data_counter = 0;
				alt_up_audio_disable_write_interrupt(audio_dev);
			}
		}
	// finally, we write this sample data to the FIFO
	alt_up_audio_write_fifo(audio_dev, sound_buff, SAMPLE_SIZE, ALT_UP_AUDIO_LEFT);
	alt_up_audio_write_fifo(audio_dev, sound_buff, SAMPLE_SIZE, ALT_UP_AUDIO_RIGHT);
}



// opens the sound file and loads it into memory
void load_sound_data()
{
	short int file_handle;
	unsigned int loop_counter;

	printf("Reading sound file...\n");

	// create large buffer to store all sound data
	begin_data = (unsigned int*) malloc(BEGIN_SIZE * sizeof(unsigned int));
	lose_data = (unsigned int*) malloc(LOSE_SIZE * sizeof(unsigned int));
	victory_data = (unsigned int*) malloc(VICTORY_SIZE * sizeof(unsigned int));
	background_data = (unsigned int*) malloc(BACKGROUND_SIZE * sizeof(unsigned int));

	printf("Check\n");
/*
	//begin
	file_handle = alt_up_sd_card_fopen("begin.wav", false);
	// skip header
	for (loop_counter = 0; loop_counter < HEADER_SIZE; loop_counter++)
	{
		alt_up_sd_card_read(file_handle);
	}
	// read and store sound data into memory
	for (loop_counter = 0; loop_counter < BEGIN_SIZE; loop_counter++)
	{
		begin_data[loop_counter] = alt_up_sd_card_read(file_handle);
	}
	alt_up_sd_card_fclose(file_handle);
	printf("Check\n");

	//victory
	file_handle = alt_up_sd_card_fopen("victory.wav", false);
	// skip header
	for (loop_counter = 0; loop_counter < HEADER_SIZE; loop_counter++)
	{
		alt_up_sd_card_read(file_handle);
	}
	// read and store sound data into memory
	for (loop_counter = 0; loop_counter < VICTORY_SIZE; loop_counter++)
	{
		victory_data[loop_counter] = alt_up_sd_card_read(file_handle);
	}
	alt_up_sd_card_fclose(file_handle);
	printf("Check\n");
*/
	//background
	file_handle = alt_up_sd_card_fopen("back.wav", false);
	// skip header
	for (loop_counter = 0; loop_counter < HEADER_SIZE; loop_counter++)
	{
		alt_up_sd_card_read(file_handle);
	}
	// read and store sound data into memory
	for (loop_counter = 0; loop_counter < BACKGROUND_SIZE; loop_counter++)
	{
		background_data[loop_counter] = alt_up_sd_card_read(file_handle);
	}
	alt_up_sd_card_fclose(file_handle);
	printf("Check\n");
/*
	//lose
	file_handle = alt_up_sd_card_fopen("defeat.wav", false);
	// skip header
	for (loop_counter = 0; loop_counter < HEADER_SIZE; loop_counter++)
	{
		alt_up_sd_card_read(file_handle);
	}
	// read and store sound data into memory
	for (loop_counter = 0; loop_counter < LOSE_SIZE; loop_counter++)
	{
		lose_data[loop_counter] = alt_up_sd_card_read(file_handle);
	}
	alt_up_sd_card_fclose(file_handle);
	printf("Check\n");

	//bullet
	file_handle = alt_up_sd_card_fopen("bullet.wav", false);
	// skip header
	for (loop_counter = 0; loop_counter < HEADER_SIZE; loop_counter++)
	{
		alt_up_sd_card_read(file_handle);
	}
	// read and store sound data into memory
	for (loop_counter = 0; loop_counter < BULLET_SIZE; loop_counter++)
	{
		lose_data[loop_counter] = alt_up_sd_card_read(file_handle);
	}
	alt_up_sd_card_fclose(file_handle);
	printf("Check\n");

	//hit
	file_handle = alt_up_sd_card_fopen("hit.wav", false);
	// skip header
	for (loop_counter = 0; loop_counter < HEADER_SIZE; loop_counter++)
	{
		alt_up_sd_card_read(file_handle);
	}
	// read and store sound data into memory
	for (loop_counter = 0; loop_counter < HIT_SIZE; loop_counter++)
	{
		lose_data[loop_counter] = alt_up_sd_card_read(file_handle);
	}
	alt_up_sd_card_fclose(file_handle);
	printf("Check\n");
*/
	// create buffer for storing samples from sound_data
	sound_buff = (unsigned int*) malloc(SAMPLE_SIZE * sizeof(unsigned int));
}

void playBegin()
{
	// register isr
	sound_data_counter = 0;
	alt_irq_register(6, 0x0, begin_isr_0);
	// enable interrupt
	alt_irq_enable(6);
	alt_up_audio_enable_write_interrupt(audio_dev);
}

void playLose()
{
	// register isr
	sound_data_counter = 0;
	alt_irq_register(6, 0x0, lose_isr_0);
	// enable interrupt
	alt_irq_enable(6);
	alt_up_audio_enable_write_interrupt(audio_dev);
}

void playBackground()
{
	// register isr
	sound_data_counter = 0;
	alt_irq_register(6, 0x0, background_isr_0);
	// enable interrupt
	alt_irq_enable(6);
	alt_up_audio_enable_write_interrupt(audio_dev);
}

void playVictory()
{
	// register isr
	sound_data_counter = 0;
	alt_irq_register(6, 0x0, victory_isr_0);
	// enable interrupt
	alt_irq_enable(6);
	alt_up_audio_enable_write_interrupt(audio_dev);
}
void playBullet()
{
	// register isr
	sound_data_counter = 0;
	alt_irq_register(6, 0x0, bullet_isr_0);
	// enable interrupt
	alt_irq_enable(6);
	alt_up_audio_enable_write_interrupt(audio_dev);
}
void playHit()
{
	// register isr
	sound_data_counter = 0;
	alt_irq_register(6, 0x0, hit_isr_0);
	// enable interrupt
	alt_irq_enable(6);
	alt_up_audio_enable_write_interrupt(audio_dev);
}
void disableSound()
{
	printf("disable\n");
	sound_data_counter = 0;
	alt_up_audio_disable_write_interrupt(audio_dev);
	alt_up_audio_reset_audio_core(audio_dev);
}

/*
void loadFromSDCard()
{
	short int file_handle = alt_up_sd_card_fopen("save.dat", false);
	if(file_handle<0){
		printf("Fail to load data!");
		return;
	}

	int i;
	char data[SAVE_FILE_SIZE];
	// skip header

	for (i = 0; i < SAVE_FILE_SIZE; i++)
	{
		data[i] = alt_up_sd_card_read(file_handle);
		//printf("%d\n",data[i]);
	}
	system_settings.audio = data[0];
	system_settings.level = data[1];
	system_settings.num_player = data[2];

	p.x = data[3]*10 + data[4];
	p.y = data[5]*10 + data[6];

	p.sizeX = data[7];
	p.sizeY = data[8];
	p.lives = data[9];
	p.alive = data[10];
	p.bulletDir = data[11];
	p.dir = data[12];

	p.image = index2image(data[13]);

	bo.x = data[14]*10 + data[15];
	bo.y = data[16]*10 + data[17];
	bo.hp = data[18];
	bo.dirX = data[19];
	bo.dirY = data[20];
	bo.bulletDir = data[21];
	bo.shootCount = data[22];

	system_settings.play_time = data[23]*100+data[24];
	system_settings.mode = data[25];

	alt_up_sd_card_fclose(file_handle);

}

void saveToSDCard(){
	short int file_handle = alt_up_sd_card_fopen("save.dat", false);

	char data[SAVE_FILE_SIZE];

	if(file_handle<0){
		printf("Fail to save data!");
		return;
	}

	data[0] = system_settings.audio;
	data[1] = system_settings.level;
	data[2] = system_settings.num_player;

	data[3] = p.x/10;
	data[4] = p.x-10*p.x/10;
	data[5] = p.y/10;
	data[6] = p.y-10*p.y/10;
	data[7] = p.sizeX;
	data[8] = p.sizeY;
	data[9] = p.lives;
	data[10] = p.alive;
	data[11] = p.bulletDir;
	data[12] = p.dir;

	//data[13] = p.image/10000;
	//data[14] = (p.image - data[13]*10000)/100;
	//data[15] = (p.image - data[13]*10000)%100;
	data[13] = image2index(p.image);

	data[14] = bo.x/10;
	data[15] = bo.x-10*bo.x/10;
	data[16] = bo.y/10;
	data[17] = bo.y-10*bo.y/10;
	data[18] = bo.hp;
	data[19] = bo.dirX;
	data[20] = bo.dirY;
	data[21] = bo.bulletDir;
	data[22] = bo.shootCount;

	//System settings 2
	int time = system_settings.play_time;
	if(time>9999)
		time = 9999;
	data[23] = time/100;
	data[24] = time%100;
	data[25] = system_settings.mode;


	int i;
	for(i=0; i<SAVE_FILE_SIZE; i++){
		alt_up_sd_card_write(file_handle, data[i]);
	}

	alt_up_sd_card_fclose(file_handle);


}
*/




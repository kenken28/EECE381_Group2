/*
 * sound_manager.h
 *
 *  Created on: 2013-10-08
 *      Author: EECE381 G11
 */


#define HEADER_SIZE 44
#define SAMPLE_SIZE 96
#define BEGIN_SIZE 68442
#define LOSE_SIZE 72674
#define VICTORY_SIZE 62476
#define BACKGROUND_SIZE 548236
#define BULLET_SIZE 26444
#define HIT_SIZE 24044


void av_config_setup();
void initialize_audio();
void initialWriteToFIFO();

void initialize_audio_irq();
//void audio_isr(void* context, alt_u32 id);
void load_sound_data();

/*
void begin_isr_0(void* context, alt_u32 id);
void victory_isr_0(void* context, alt_u32 id);
void lose_isr_0(void* context, alt_u32 id);
void background_isr_0(void* context, alt_u32 id);
void bullet_isr_0(void* context, alt_u32 id);
void hit_isr_0(void* context, alt_u32 id);
*/

void loadFromSDCard();
void saveToSDCard();

void playBegin();
void playLose();
void playBackground();
void playVictory();
void playBullet();
void playHit();
void disableSound();



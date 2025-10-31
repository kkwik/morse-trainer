#include "morse_player.h"
#include "miniaudio/miniaudio.c"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

#define DEVICE_FORMAT ma_format_f32
#define DEVICE_CHANNELS 2
#define DEVICE_SAMPLE_RATE 48000

ma_device *device;
static ma_waveform *g_symbolDataSources;
static size_t *g_symbolDataSources_length;
ma_event g_stopEvent;
mtx_t serial_mtx;

void data_callback(ma_device *pDevice, void *pOutput, const void *pInput,
                   ma_uint32 frameCount) {
  ma_decoder *pDecoder = (ma_decoder *)pDevice->pUserData;
  if (pDecoder == NULL) {
    return;
  }
  printf("Callback called\n");

  unsigned long long framesRead;
  ma_data_source_read_pcm_frames(pDecoder, pOutput, frameCount, &framesRead);
  if (framesRead < frameCount) {
    ma_event_signal(&g_stopEvent);
  }

  (void)pInput;
}

bool player_setup(size_t max_code_length) {
  // mtx_init(&serial_mtx, mtx_plain);
  ma_device_config deviceConfig;
  deviceConfig = ma_device_config_init(ma_device_type_playback);
  deviceConfig.playback.format = DEVICE_FORMAT;
  deviceConfig.playback.channels = DEVICE_CHANNELS;
  deviceConfig.sampleRate = DEVICE_SAMPLE_RATE;
  deviceConfig.dataCallback = data_callback;
  deviceConfig.pUserData = NULL;

  device = malloc(sizeof(*device));
  ma_result result = ma_device_init(NULL, &deviceConfig, device);
  if (result != MA_SUCCESS) {
    printf("ERROR: Failed to initialize device.");
  }

  // Allocate enough space for the maximum code length
  g_symbolDataSources_length = malloc(sizeof(g_symbolDataSources_length));
  *g_symbolDataSources_length = 2 * max_code_length;
  g_symbolDataSources =
      malloc(*g_symbolDataSources_length * sizeof(*g_symbolDataSources));
  return true;
}

void play_morse_char(struct player_config config) {
  char *code = config.code;
  (void)code;

  // ma_result result;
  int ditMS = 100; // TODO: real value
  int ditPCM = (ditMS * DEVICE_SAMPLE_RATE) / 1000;

  ma_waveform_config sineWaveConfig = ma_waveform_config_init(
      DEVICE_FORMAT, DEVICE_CHANNELS, DEVICE_SAMPLE_RATE, ma_waveform_type_sine,
      config.amp, config.hz);

  // WARN: This doesn't work threaded
  ma_waveform wave;
  ma_waveform *fwave = &wave;
  ma_waveform_init(&sineWaveConfig, fwave);
  ma_data_source_set_range_in_pcm_frames(fwave, 0, ditPCM);
  ma_device_config deviceConfig;
  deviceConfig = ma_device_config_init(ma_device_type_playback);
  deviceConfig.playback.format = DEVICE_FORMAT;
  deviceConfig.playback.channels = DEVICE_CHANNELS;
  deviceConfig.sampleRate = DEVICE_SAMPLE_RATE;
  deviceConfig.dataCallback = data_callback;
  deviceConfig.pUserData = fwave;

  ma_device d2;
  ma_device_init(NULL, &deviceConfig, &d2);
  ma_event_init(&g_stopEvent);
  ma_device_start(&d2);

  // Wait for playback to finish before cleanup
  // ma_event_wait(&g_stopEvent);
  getchar();
  ma_device_stop(&d2);
  ma_waveform_uninit(fwave);

  // WARN: Engine does work
  // ma_engine engine;
  // ma_engine_init(NULL, &engine);
  // ma_engine_play_sound(&engine, "test.wav", NULL);
  // getchar();
  // ma_engine_uninit(&engine);

  // for (size_t i = 0; i < strlen(code); i++) {
  //   // For each mark in code also create a short gap afterwards
  //   ma_waveform *wave = &g_symbolDataSources[2 * i];
  //   ma_waveform_init(&sineWaveConfig, wave);
  //   int markPCM = code[i] == '.' ? ditPCM : 3 * ditPCM;
  //   ma_data_source_set_range_in_pcm_frames(wave, 0, markPCM);
  //
  //   ma_waveform *gap = &g_symbolDataSources[(2 * i) + 1];
  //   ma_waveform_init(&sineWaveConfig, gap);
  //   ma_data_source_set_range_in_pcm_frames(gap, 0, ditPCM);
  //   ma_waveform_set_amplitude(gap, 0.0);
  //
  //   // Chain playback of data sources
  //   if (i != 0) {
  //     ma_data_source_set_next(&g_symbolDataSources[(2 * i) - 1], wave);
  //   }
  //   ma_data_source_set_next(wave, gap);
  // }

  {
    // device->pUserData = fwave;
    // ma_event_init(&g_stopEvent);
    // result = ma_device_start(device);
    // if (result != MA_SUCCESS) {
    //   printf("ERROR: Failed to start device.");
    //   ma_device_uninit(device);
    // }
    //
    // // Wait for playback to finish before cleanup
    // ma_event_wait(&g_stopEvent);
    // result = ma_device_stop(device);
    // if (result != MA_SUCCESS) {
    //   printf("ERROR: Failed to stop device.");
    //   ma_device_uninit(device);
    // }

    // for (size_t i = 0; i < *g_symbolDataSources_length; i++) {
    //   ma_waveform_uninit(&g_symbolDataSources[i]);
    // }
  }
}
int thread_play_morse_char(void *arg) {
  (void)arg;
  ma_waveform sineWave;
  ma_device_config deviceConfig;
  ma_device device;
  ma_waveform_config sineWaveConfig;

  deviceConfig = ma_device_config_init(ma_device_type_playback);
  deviceConfig.playback.format = DEVICE_FORMAT;
  deviceConfig.playback.channels = DEVICE_CHANNELS;
  deviceConfig.sampleRate = DEVICE_SAMPLE_RATE;
  deviceConfig.dataCallback = data_callback;
  deviceConfig.pUserData = &sineWave;

  if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
    printf("Failed to open playback device.\n");
    return -4;
  }

  sineWaveConfig = ma_waveform_config_init(
      device.playback.format, device.playback.channels, device.sampleRate,
      ma_waveform_type_sine, 0.2, 220);
  ma_waveform_init(&sineWaveConfig, &sineWave);
  ma_data_source_set_range_in_pcm_frames(&sineWave, 0, 10900);
  ma_event_init(&g_stopEvent);

  if (ma_device_start(&device) != MA_SUCCESS) {
    printf("Failed to start playback device.\n");
    ma_device_uninit(&device);
    return -5;
  }

  ma_event_wait(&g_stopEvent);
  printf("Thread end\n");
  ma_device_uninit(&device);
  ma_waveform_uninit(
      &sineWave); /* Uninitialize the waveform after the device so we don't pull
                     it from under the device while it's being reference in the
                     data callback. */

  return 0;
}

bool player_teardown() {
  ma_device_uninit(device);
  free(g_symbolDataSources);
  free(g_symbolDataSources_length);
  free(device);
  // mtx_destroy(&serial_mtx);

  return true;
}

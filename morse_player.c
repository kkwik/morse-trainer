#include "morse_player.h"
#include "miniaudio/miniaudio.c"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define DEVICE_FORMAT ma_format_f32
#define DEVICE_CHANNELS 2
#define DEVICE_SAMPLE_RATE 48000

static ma_waveform *g_symbolDataSources;
static size_t *g_symbolDataSources_length;
ma_event g_stopEvent;

// Allocates enough memory to represent any morse sequence in morse_table
// (including gaps)
bool player_setup(size_t max_code_length) {
  g_symbolDataSources_length = malloc(sizeof(size_t));
  *g_symbolDataSources_length = 2 * max_code_length;
  g_symbolDataSources =
      malloc(*g_symbolDataSources_length * sizeof(ma_waveform));
  return true;
}

void data_callback(ma_device *pDevice, void *pOutput, const void *pInput,
                   ma_uint32 frameCount) {
  ma_decoder *pDecoder = (ma_decoder *)pDevice->pUserData;
  if (pDecoder == NULL) {
    return;
  }

  unsigned long long framesRead;
  ma_data_source_read_pcm_frames(pDecoder, pOutput, frameCount, &framesRead);
  if (framesRead < frameCount) {
    ma_event_signal(&g_stopEvent);
  }

  (void)pInput;
}

void play_morse_char(struct player_config config, char *code) {

  ma_result result;
  int ditMS = 100; // TODO: real value
  int ditPCM = (ditMS * DEVICE_SAMPLE_RATE) / 1000;

  ma_waveform_config sineWaveConfig = ma_waveform_config_init(
      DEVICE_FORMAT, DEVICE_CHANNELS, DEVICE_SAMPLE_RATE, ma_waveform_type_sine,
      config.amp, config.hz);

  for (size_t i = 0; i < strlen(code); i++) {
    // For each mark in code also create a short gap afterwards
    ma_waveform *wave = &g_symbolDataSources[2 * i];
    ma_waveform_init(&sineWaveConfig, wave);
    int markPCM = code[i] == '.' ? ditPCM : 3 * ditPCM;
    ma_data_source_set_range_in_pcm_frames(wave, 0, markPCM);

    ma_waveform *gap = &g_symbolDataSources[(2 * i) + 1];
    ma_waveform_init(&sineWaveConfig, gap);
    ma_data_source_set_range_in_pcm_frames(gap, 0, ditPCM);
    ma_waveform_set_amplitude(gap, 0.0);

    // Chain playback of data sources
    if (i != 0) {
      ma_data_source_set_next(&g_symbolDataSources[(2 * i) - 1], wave);
    }
    ma_data_source_set_next(wave, gap);
  }

  {
    ma_device_config deviceConfig;
    ma_device device;

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = DEVICE_FORMAT;
    deviceConfig.playback.channels = DEVICE_CHANNELS;
    deviceConfig.sampleRate = DEVICE_SAMPLE_RATE;
    deviceConfig.dataCallback = data_callback;
    deviceConfig.pUserData = &g_symbolDataSources[0];

    result = ma_device_init(NULL, &deviceConfig, &device);
    if (result != MA_SUCCESS) {
      printf("ERROR: Failed to initialize device.");
    }

    ma_event_init(&g_stopEvent);
    result = ma_device_start(&device);
    if (result != MA_SUCCESS) {
      printf("ERROR: Failed to start device.");
      ma_device_uninit(&device);
    }

    // Wait for playback to finish before cleanup
    ma_event_wait(&g_stopEvent);

    ma_device_uninit(&device);
    for (size_t i = 0; i < *g_symbolDataSources_length; i++) {
      ma_waveform_uninit(&g_symbolDataSources[i]);
    }
  }
}

bool player_teardown() {
  free(g_symbolDataSources);
  free(g_symbolDataSources_length);

  return true;
}

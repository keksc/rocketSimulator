#pragma once

#include <stdexcept>
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

namespace audio {
void play(const char *path) {
  ma_result result;
  ma_engine engine;

  result = ma_engine_init(NULL, &engine);
  if (result != MA_SUCCESS) {
    throw std::runtime_error("Failed to initialize audio engine");
  }

  ma_sound sound;
  result = ma_sound_init_from_file(&engine, path, 0, NULL, NULL, &sound);
  if (result != MA_SUCCESS) {
    throw std::runtime_error("Failed to load sound file");
  }
  ma_sound_start(&sound);
  while (ma_sound_at_end(&sound) == 0) {
  }
  ma_sound_stop(&sound);
  ma_engine_uninit(&engine);
  return;
}
namespace loop {
void data_callback(ma_device *pDevice, void *pOutput, const void *pInput,
                   ma_uint32 frameCount) {
  ma_decoder *pDecoder = (ma_decoder *)pDevice->pUserData;
  if (pDecoder == NULL) {
    return;
  }

  /* Reading PCM frames will loop based on what we specified when called
   * ma_data_source_set_looping(). */
  ma_data_source_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

  (void)pInput;
}
ma_decoder decoder;
ma_device device;
void start(const char *path) {
  ma_result result;
  ma_device_config deviceConfig;

  result = ma_decoder_init_file(path, NULL, &decoder);
  if (result != MA_SUCCESS) {
    throw std::runtime_error("Failed to init miniaudio decoder");
  }

  /*
  A decoder is a data source which means we just use
  ma_data_source_set_looping() to set the looping state. We will read data using
  ma_data_source_read_pcm_frames() in the data callback.
  */
  ma_data_source_set_looping(&decoder, MA_TRUE);

  deviceConfig = ma_device_config_init(ma_device_type_playback);
  deviceConfig.playback.format = decoder.outputFormat;
  deviceConfig.playback.channels = decoder.outputChannels;
  deviceConfig.sampleRate = decoder.outputSampleRate;
  deviceConfig.dataCallback = data_callback;
  deviceConfig.pUserData = &decoder;

  if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
    throw std::runtime_error("Failed to open playback device");
  }

  if (ma_device_start(&device) != MA_SUCCESS) {
    throw std::runtime_error("Failed to start playback device");
    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);
  }
}
void stop() {
  ma_device_uninit(&device);
  ma_decoder_uninit(&decoder);
}
} // namespace loop
} // namespace audio

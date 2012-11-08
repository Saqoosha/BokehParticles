//
//  ofxSpectrumAnalyzer.h
//  VisualizeExample
//
//  Created by Saqoosha on 12/05/15.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "ofxFft.h"

#define MIN_LEVEL (1.f / 65535.f)
#define MIN_DB (20.f * log10(MIN_LEVEL))

class ofxSpectrumAnalyzer : public ofBaseSoundInput {
 public:
  ofxSpectrumAnalyzer() : num_subbands_(16), volume_(MIN_DB), smoothed_volume_(MIN_DB), gain_(0) {};
  
  void setup(int buffer_size = 512, int num_subbands = 16);
  void drawTimeDomain(float x, float y, float width, float height);
  void drawFrequencyDomain(float x, float y, float width, float height, bool log_scale = true);
  void drawSpectrogram(float x, float y, float width, float height);
  void drawBandVolumes(float x, float y, float width, float height);
  void plot(vector<float> &buffer);
  void audioIn(float *input, int buffer_size, int num_channels);
  
  inline void set_gain(float gain) { gain_ = gain; };
  inline int num_subbands() { return num_subbands_; };
  
  inline float volume(bool normalized = true) {
    return normalized ? ofMap(volume_, MIN_DB, 0, 0, 1) : volume_;
  };
  inline float smoothed_volume(bool normalized = true) {
    return normalized ? ofMap(smoothed_volume_, MIN_DB, 0, 0, 1) : smoothed_volume_;
  };
  inline float subband_volume(int band, bool normalized = true) {
    return normalized ? ofMap(band_volume_[band], MIN_DB, 0, 0, 1) : band_volume_[band];
  };
  inline float smoothed_subband_volume(int band, bool normalized = true) {
    return normalized ? ofMap(smoothed_band_volume_[band], MIN_DB, 0, 0, 1) : smoothed_band_volume_[band];
  };
  
 protected:
  inline float getAmplitudeAtFrequency(vector<float> &amplitude, float frequency) {
    int size = fft_->getBinSize();
    float bin = frequency / 22010.f * size;
    int lowBin = ofClamp(floorf(bin), 0, size - 1 - 1e-3);
    int highBin = ofClamp(ceilf(bin), 1e-3, size - 1);
    return ofMap(bin, lowBin, highBin, amplitude[lowBin], amplitude[highBin]);
  };
  
 private:
  ofSoundStream sound_stream_;
  ofxFft *fft_;
  int num_subbands_;
  
  ofMutex sound_mutex_;
  vector<float> draw_buffer_, middle_buffer_, audio_buffer_;
  vector<float> draw_bins_, middle_bins_, audio_bins_;
  vector<float> awindow_;
  
  ofImage spectrogram_;
  int spectrogram_offset_;
  
  // volumes in dBFS
  float volume_;
  float smoothed_volume_;
  vector<float> band_volume_;
  vector<float> smoothed_band_volume_;

  float gain_;
};

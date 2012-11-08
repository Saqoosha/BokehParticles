//
//  ofxSpectrumAnalyzer.cpp
//  VisualizeExample
//
//  Created by Saqoosha on 12/05/15.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#include "ofxSpectrumAnalyzer.h"


#define FQMIN (20.f)
#define FQMAX (22050.f)
#define LOGMIN log10(FQMIN)
#define LOGMAX log10(FQMAX)


//------------------------------------------------------------------------------
void ofxSpectrumAnalyzer::setup(int buffer_size, int num_subbands) {
  //  fft = ofxFft::create(bufferSize, OF_FFT_WINDOW_HAMMING);
  // To use FFTW, try:
  fft_ = ofxFft::create(buffer_size, OF_FFT_WINDOW_HAMMING, OF_FFT_FFTW);
  
  spectrogram_.allocate(512, fft_->getBinSize(), OF_IMAGE_GRAYSCALE);
  memset(spectrogram_.getPixels(), 0, (int)(spectrogram_.getWidth() * spectrogram_.getHeight()));
  spectrogram_offset_ = 0;
  
  draw_buffer_.resize(buffer_size);
  middle_buffer_.resize(buffer_size);
  audio_buffer_.resize(buffer_size);
  
  draw_bins_.resize(fft_->getBinSize());
  middle_bins_.resize(fft_->getBinSize());
  audio_bins_.resize(fft_->getBinSize());
  
  // A-weighting window pre-calculation
  // http://www.diracdelta.co.uk/science/source/a/w/aweighting/source.html
  awindow_.resize(fft_->getBinSize());
  for (int i = 0; i < awindow_.size(); i++) {
    float freq = (float)i / fft_->getBinSize() * (44100.f / 2.f);
    float f2 = freq * freq;
    float f4 = f2 * f2;
    double w = 10 * log10(1.562339 * f4 / ((f2 + 11589.09305) * (f2 + 544440.6705)));
    w += 10 * log10(2.242881e16 * f4 / (pow(f2 + 424.3186774, 2) * pow(f2 + 148699001.4, 2)));
    awindow_[i] = w; // db scale
  }
  
  sound_stream_.setInput(this);
  sound_stream_.setup(0, 1, 44100, buffer_size, 2);

  num_subbands_ = num_subbands;
  band_volume_.resize(num_subbands_);
  smoothed_band_volume_.resize(num_subbands_);
  for (int i = 0; i < num_subbands_; i++) {
    band_volume_[i] = MIN_DB;
    smoothed_band_volume_[i] = MIN_DB;
  }
}


//------------------------------------------------------------------------------
void ofxSpectrumAnalyzer::drawTimeDomain(float x, float y, float width, float height) {
  sound_mutex_.lock();
    draw_buffer_ = middle_buffer_;
  sound_mutex_.unlock();
  
  ofPushMatrix();
    ofTranslate(x, y + height / 2);
    ofScale(width, height / 2);
    plot(draw_buffer_);
  ofPopMatrix();
}


//------------------------------------------------------------------------------
void ofxSpectrumAnalyzer::drawFrequencyDomain(float x, float y, float width, float height, bool log_scale) {
  sound_mutex_.lock();
    draw_bins_ = middle_bins_;
  sound_mutex_.unlock();
  
  ofPushMatrix();
    ofTranslate(x, y);
    int n = fft_->getBinSize();
    ofBeginShape();
    for (int i = 0; i < width; i++) {
      float fq = pow(10.f, ofMap(i, 0, width - 1, LOGMIN, LOGMAX));
      ofVertex(i, ofMap(20.f * log10(getAmplitudeAtFrequency(draw_bins_, fq)), MIN_DB, 0, height, 0, true));
    }
    ofEndShape();
  ofPopMatrix();
}


//------------------------------------------------------------------------------
void ofxSpectrumAnalyzer::drawSpectrogram(float x, float y, float width, float height) {
  spectrogram_.update();
  spectrogram_.draw(x, y, width, height);
}


//------------------------------------------------------------------------------
void ofxSpectrumAnalyzer::drawBandVolumes(float x, float y, float width, float height) {
  ofPushMatrix();
    ofTranslate(x, y + height);
    ofScale(1, -1);
    float w = width / num_subbands_;
    for (int i = 0; i < num_subbands_; i++) {
      ofRect((float)i * w, 0, w, ofMap(smoothed_band_volume_[i], MIN_DB, 0, 0, height, true));
    }
  ofPopMatrix();
}


//------------------------------------------------------------------------------
void ofxSpectrumAnalyzer::plot(vector<float> &buffer) {
  int n = buffer.size();
  ofBeginShape();
    for (int i = 0; i < n; i++) {
      ofVertex((float)i / n, buffer[i]);
    }
  ofEndShape();
}


//------------------------------------------------------------------------------
void ofxSpectrumAnalyzer::audioIn(float *input, int buffer_size, int num_channels) {
  memcpy(&audio_buffer_[0], input, sizeof(float) * buffer_size);
  for (int i = 0; i < buffer_size; i++) {
    float a = input[i];
    float aa = abs(a);
    float db = 20.f * log10(aa) + gain_;
    input[i] = pow(10.f, db / 20.f) * a / abs(a);
  }
  
  fft_->setSignal(input);
  memcpy(&audio_bins_[0], fft_->getAmplitude(), sizeof(float) * fft_->getBinSize());
  
  volume_ = 0;
  int count = 0;
  memset(&band_volume_[0], 0, sizeof(int) * num_subbands_);
  int band_count[num_subbands_];
  memset(band_count, 0, sizeof(int) * num_subbands_);
  for (int i = 0; i < fft_->getBinSize(); i++) {
    float db = 20.f * log10(audio_bins_[i]) + awindow_[i];
    float a = pow(pow(10.f, db / 20.f), 2.f);
    if (a > 0.0001f) {
      volume_ += a * a;
      count++;
      float fq = ofClamp(ofMap(i, 0, fft_->getBinSize() - 1, 1, 22050), FQMIN, FQMAX);
      int band = min(floor(ofMap(log10(fq), LOGMIN, LOGMAX, 0, num_subbands_)), (float)num_subbands_ - 1);
      band_volume_[band] += a * a;
      band_count[band]++;
    }
    audio_bins_[i] = a;
  }
  volume_ = sqrt(volume_ / (float)count);
  if (volume_ != volume_) volume_ = MIN_LEVEL;
  volume_ = 20.f * log10(max(volume_, MIN_LEVEL));
  float a = volume_ > smoothed_volume_ ? .3f : .1f;
  smoothed_volume_ *= 1.f - a;
  smoothed_volume_ += a * volume_;
  
  for (int i = 0; i < num_subbands_; i++) {
    band_volume_[i] = sqrt(band_volume_[i] / (float)band_count[i]);
    if (band_volume_[i] != band_volume_[i]) band_volume_[i] = MIN_LEVEL;
    band_volume_[i] = 20.f * log10(max(band_volume_[i], MIN_LEVEL));
    float a = band_volume_[i] > smoothed_band_volume_[i] ? .6f : .06f;
    smoothed_band_volume_[i] *= 1.f - a;
    smoothed_band_volume_[i] += a * band_volume_[i];
  }
  
  int width = spectrogram_.getWidth();
  int height = spectrogram_.getHeight();
  unsigned char *pixels = spectrogram_.getPixels();
  for (int i = 0; i < height; i++) {
    float fq = pow(10.f, ofMap(i, 0, height - 1, LOGMAX, LOGMIN));
    float db = 20.f * log10(getAmplitudeAtFrequency(audio_bins_, fq));
    pixels[i * width + spectrogram_offset_] = 255.f * ofMap(db, MIN_DB, 0, 0, 1, true);
  }
  spectrogram_offset_ = (spectrogram_offset_ + 1) % width;
  
  sound_mutex_.lock();
    middle_buffer_ = audio_buffer_;
    middle_bins_ = audio_bins_;
  sound_mutex_.unlock();
}



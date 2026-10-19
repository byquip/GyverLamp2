#pragma once
#include <Arduino.h>

class Clap {
public:
    // Call as often as possible with the latest sample
    inline void tick(int val) {
        const uint32_t now = micros();

        // fixed sample period
        if ((uint32_t)(now - _sampleTmr) < _sampleUs) return;
        _sampleTmr = now;

        const int der = val - _prevVal;
        _prevVal = val;

        int8_t signal = 0;
        if (der > _trsh) signal = 1;
        else if (der < -_trsh) signal = -1;

        int8_t front = 0;
        if (_prevSignal == 0) front = signal;
        _prevSignal = signal;

        const uint32_t dt = now - _eventTmr;

        switch (_state) {
            case 0: // idle, wait for rising edge
                if (front == 1) {
                    _state = 1;
                    if (!_startClap) {
                        _claps = 0;
                        _ready = false;
                    }
                    _startClap = true;
                    _clap = false;
                    _eventTmr = now;
                }
                break;

            case 1: // got positive front, wait for negative front
                if (front == -1) {
                    if (dt <= _pulseWindowUs) {
                        _state = 2;
                        _eventTmr = now;
                    } else {
                        _state = 0;
                    }
                } else if (dt > _pulseWindowUs) {
                    _state = 0;
                }
                break;

            case 2: // got negative front, wait for signal to settle
                if (front == 0) {
                    if (dt <= _pulseWindowUs) {
                        _state = 0;
                        ++_claps;
                        _clap = true;
                        _eventTmr = now;
                    } else {
                        _state = 0;
                    }
                } else if (dt > _pulseWindowUs) {
                    _state = 0;
                }
                break;
        }

        if (_startClap && (uint32_t)(now - _eventTmr) > _toutUs) {
            _state = 0;
            _startClap = false;
            if (_claps) _ready = true;
        }
    }

    inline void setTrsh(int trsh) {
        _trsh = trsh;
    }

    // timeout between clap series, in ms
    inline void setTimeout(uint16_t toutMs) {
        _toutUs = (uint32_t)toutMs * 1000UL;
    }

    // sample period, in us
    inline void setSampleUs(uint16_t us) {
        _sampleUs = us;
    }

    // pulse window for one clap shape, in ms
    inline void setPulseWindow(uint16_t ms) {
        _pulseWindowUs = (uint32_t)ms * 1000UL;
    }

    inline bool isClap() {
        if (_clap) {
            _clap = false;
            return true;
        }
        return false;
    }

    inline bool hasClaps(uint8_t claps) {
        if (_ready && _claps == claps) {
            _ready = false;
            _claps = 0;
            return true;
        }
        return false;
    }

    inline bool hasClaps() const {
        return _ready;
    }

    inline uint8_t getClaps() {
        if (_ready) {
            _ready = false;
            const uint8_t n = _claps;
            _claps = 0;
            return n;
        }
        return 0;
    }

private:
    uint32_t _sampleTmr = 0;
    uint32_t _eventTmr = 0;

    int _prevVal = 0;
    int _trsh = 150;

    uint32_t _sampleUs = 1000UL;        // 1 ms sampling
    uint32_t _pulseWindowUs = 120000UL; // 120 ms
    uint32_t _toutUs = 700000UL;        // 700 ms

    uint8_t _state = 0;
    uint8_t _claps = 0;

    int8_t _prevSignal = 0;

    bool _ready = false;
    bool _clap = false;
    bool _startClap = false;
};
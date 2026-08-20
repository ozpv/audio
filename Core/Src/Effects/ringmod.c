/*
 * ringmod.c
 *
 *  Created on: Aug 19, 2026
 *      Author: ozpv
 */

#include "cs4272.h"
#include "ringmod.h"
#include <math.h>

/* ring modulator */
void ringmod(float *samples, size_t range_min, size_t range_max) {
    static float lfo_phase = 0.0f;
    static float carrier_phase = 0.0f;

    const float frequency = 156.0f;
    const float amount = 0.67f;
    const float sample_rate = (float)SAMPLE_RATE;

    float lfo_increment = M_TWOPI * (0.18f / sample_rate);

    /* plus two because of */
    for (size_t i = range_min; i < range_max; i += 2) {
        lfo_phase = fmodf(lfo_phase + lfo_increment, M_TWOPI);

        float lfo = sinf(lfo_phase);

        float carrier_increment = M_TWOPI * ((frequency + lfo * (frequency * 3.0f * amount)) / sample_rate);

        carrier_phase = fmodf(carrier_phase + carrier_increment, M_TWOPI);

        float carrier = sinf(carrier_phase);

        samples[i] = (samples[i] * 0.50f) + (samples[i] * carrier * 0.50f);
    }
}

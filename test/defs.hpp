/*
 *  test/defs.h
 *  Copyright 2020-2021 ItJustWorksTM
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#ifndef LIBSMCE_TEST_MACRODEFS_H
#define LIBSMCE_TEST_MACRODEFS_H

#define SMCE_PATH SMCE_TEST_DIR "/smce_root"
#define SKETCHES_PATH SMCE_TEST_DIR "/sketches/"
#define PATCHES_PATH SMCE_TEST_DIR "/patches/"
#define STORAGE_PATH SMCE_TEST_DIR "/storage/"
#define MANIFESTS_PATH SMCE_TEST_DIR "/manifests/"

#include <thread>

template <class Pin, class Value, class Duration>
void test_pin_delayable(Pin pin, Value expected_value, std::size_t ticks, Duration tick_length) {
    do {
        if (ticks-- == 0)
            FAIL("Timed out pin-wait");
        std::this_thread::sleep_for(tick_length);
    } while (pin.read() != expected_value);
}

#endif // LIBSMCE_TEST_MACRODEFS_H

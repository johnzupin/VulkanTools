/*
 * Copyright (c) 2020 Valve Corporation
 * Copyright (c) 2020 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Authors:
 * - Christophe Riccio <christophe@lunarg.com>
 */

#include "layer_setting_data.h"
#include "util.h"

#include <cassert>
#include <algorithm>

LayerSettingData::LayerSettingData(const char* key, const char* value) : key(key), value(value) {
    assert(key != nullptr);
    assert(value != nullptr);
}

bool HasPreset(const std::vector<LayerSettingData>& layer_settings, const std::vector<LayerSettingData>& preset_settings) {
    if (preset_settings.empty()) return false;

    for (std::size_t preset_index = 0, preset_count = preset_settings.size(); preset_index < preset_count; ++preset_index) {
        const LayerSettingData* layer_setting = FindByKey(layer_settings, preset_settings[preset_index].key.c_str());
        if (layer_setting == nullptr) return false;

        if (preset_settings[preset_index].value != layer_setting->value) return false;
    }

    return true;
}

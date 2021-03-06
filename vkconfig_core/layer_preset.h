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

#pragma once

#include "platform.h"
#include "layer_setting_data.h"

#include <vector>
#include <string>

struct LayerPreset {
    int preset_index;
    std::string label;
    std::string description;
    int platform_flags;
    StatusType status_type;
    std::vector<LayerSettingData> settings;
};

const LayerPreset* GetPreset(const std::vector<LayerPreset>& presets, int preset_index);

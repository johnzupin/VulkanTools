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
 * - Richard S. Wright Jr. <richard@lunarg.com>
 * - Christophe Riccio <christophe@lunarg.com>
 */

#pragma once

#include "../vkconfig_core/layer.h"
#include "../vkconfig_core/layer_manager.h"
#include "../vkconfig_core/path_manager.h"
#include "../vkconfig_core/environment.h"
#include "../vkconfig_core/configuration.h"
#include "../vkconfig_core/platform.h"

#include <QString>
#include <QDir>
#include <QTreeWidget>

#include <vulkan/vulkan.h>

#include <vector>

// This is a master list of layer settings. All the settings
// for what layers can have user modified settings. It contains
// the names of the layers and the properties of the settings.
// THIS IS TO BE READ ONLY, as it is copied from frequently
// to reset or initialize the a full layer definition for the
// profiles.
struct LayerSettingsDefaults {
    QString layer_name;                  // Name of layer
    std::vector<LayerSetting> settings;  // Default settings for this layer
};

class Configurator {
   public:
    static Configurator& Get();
    bool Init();

    // Validation Preset
   public:
    const char* GetValidationPresetName(ValidationPreset preset) const;
    const char* GetValidationPresetLabel(ValidationPreset preset) const;

    // Additional places to look for layers
   public:
    void BuildCustomLayerTree(QTreeWidget* tree_widget);

    // The list of applications affected
   public:
    // If return_loader_version is not null, the function will return the loader version
    // If quiet is false, message box will be generate
    bool SupportApplicationList(Version* return_loader_version = nullptr) const;

    bool HasActiveOverrideOnApplicationListOnly() const {
        return SupportApplicationList() && environment.UseApplicationListOverrideMode();
    }

    // A readonly list of layer names with the associated settings
    // and their default values. This is for reference by individual profile
    // objects.
    std::vector<LayerSettingsDefaults> _default_layers_settings;
    void LoadDefaultLayerSettings();
    const LayerSettingsDefaults* FindLayerSettings(const QString& layer_name) const;

    std::vector<Configuration> available_configurations;
    void LoadAllConfigurations();  // Load all the .profile files found
    void ImportConfiguration(const QString& full_import_path);
    void ExportConfiguration(const QString& source_file, const QString& full_export_path);
    void ResetDefaultsConfigurations();

    bool HasLayers() const;

    // Set this as the current override configuration
    std::vector<Configuration>::iterator GetActiveConfiguration() const { return _active_configuration; }
    void SetActiveConfiguration(std::vector<Configuration>::iterator active_configuration);
    void SetActiveConfiguration(const QString& configuration_name);
    void RefreshConfiguration();
    void RemoveConfiguration(const QString& configuration_name);
    bool HasActiveConfiguration() const;

   private:
    Configurator();
    ~Configurator();

    Configurator(const Configurator&) = delete;
    Configurator& operator=(const Configurator&) = delete;

    std::vector<Configuration>::iterator _active_configuration;

   public:
    PathManager path;
    Environment environment;
    LayerManager layers;
};

ValidationPreset GetValidationPreset(const QString& configuration_name);

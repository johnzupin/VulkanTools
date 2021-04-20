/*
 * Copyright (c) 2020-2021 Valve Corporation
 * Copyright (c) 2020-2021 LunarG, Inc.
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

#include "../vkconfig_core/layer.h"
#include "../vkconfig_core/parameter.h"

#include <QObject>
#include <QWidget>
#include <QComboBox>
#include <QTreeWidgetItem>

#include <vector>

class WidgetPreset : public QComboBox {
    Q_OBJECT

   public:
    explicit WidgetPreset(QTreeWidgetItem* item, const Layer& layer, Parameter& parameter);

   public Q_SLOTS:
    void OnPresetChanged(int index);

   Q_SIGNALS:
    void itemChanged();

   protected:
    void paintEvent(QPaintEvent* event) override;

   private:
    WidgetPreset(const WidgetPreset&) = delete;
    WidgetPreset& operator=(const WidgetPreset&) = delete;

    int GetComboBoxIndex(const char* preset_label) const;

    std::vector<std::string> preset_labels;  // The preset in the combobox
    const Layer& layer;
    Parameter& parameter;
};

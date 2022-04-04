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

#include "help.h"
#include "util.h"
#include "platform.h"

#include <QDesktopServices>
#include <QUrl>

#include <cassert>

void ShowDoc(DocType doc_type) {
    static const char* PLATFORM_STRING[] = {
        "windows",  // PLATFORM_WINDOWS
        "linux",    // PLATFORM_LINUX
        "mac"       // PLATFORM_MACOS
    };

    const char* platform = PLATFORM_STRING[VKC_PLATFORM];

    switch (doc_type) {
        default: {
            assert(0);
            break;
        }
        case DOC_FIND_LAYERS: {
            const std::string url =
                "https://github.com/KhronosGroup/Vulkan-Guide/blob/master/chapters/development_tools.adoc#vulkan-layers";
            QDesktopServices::openUrl(QUrl(url.c_str()));
            break;
        }
        case DOC_VKCONFIG_README: {
            const std::string url = format("https://vulkan.lunarg.com/doc/view/latest/%s/vkconfig.html", platform);
            QDesktopServices::openUrl(QUrl(url.c_str()));
            break;
        }
        case DOC_VKCONFIG_CHANGELOG: {
            QDesktopServices::openUrl(QUrl("https://github.com/LunarG/VulkanTools/blob/master/vkconfig/CHANGELOG.md"));
            break;
        }
        case DOC_GPU_INFO: {
            QDesktopServices::openUrl(QUrl("https://vulkan.gpuinfo.org/listreports.php"));
            break;
        }
        case DOC_VULKAN_SPEC: {
            const std::string url = format("https://vulkan.lunarg.com/doc/view/latest/%s/1.2-extensions/vkspec.html", platform);
            QDesktopServices::openUrl(QUrl(url.c_str()));
            break;
        }
        case DOC_VULKAN_LAYERS: {
            const std::string url = format("https://vulkan.lunarg.com/doc/view/latest/%s/layer_configuration.html", platform);
            QDesktopServices::openUrl(QUrl(url.c_str()));
            break;
        }
    }
}

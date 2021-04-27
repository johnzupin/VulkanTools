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
 * - Richard S. Wright Jr. <richard@lunarg.com>
 * - Christophe Riccio <christophe@lunarg.com>
 */

#include "widget_setting_list.h"
#include "widget_setting_list_element.h"
#include "widget_setting.h"

#include "../vkconfig_core/util.h"

#include <QMessageBox>

#include <cassert>

const char *GetFieldToolTip(const SettingMetaList &meta, bool current_list_empty) {
    if (meta.list.empty()) {
        return "Start tapping to add a new value";
    } else if (meta.list_only && current_list_empty) {
        return "All the accepted values are already listed";
    } else {
        return "Start tapping to search for available values";
    }
}

WidgetSettingList::WidgetSettingList(QTreeWidget *tree, QTreeWidgetItem *item, const SettingMetaList &meta,
                                     SettingDataSet &data_set)
    : WidgetSettingBase(tree, item),
      meta(meta),
      data(*data_set.Get<SettingDataList>(meta.key.c_str())),
      data_set(data_set),
      search(nullptr),
      field(new QLineEdit(this)),
      add_button(new QPushButton(this)),
      list(meta.list),
      data_cached(data.key) {
    assert(&this->meta);
    assert(&this->data);

    for (std::size_t i = 0, n = this->data.value.size(); i < n; ++i) {
        ::RemoveValue(this->list, this->data.value[i]);
    }

    const char *tooltip = GetFieldToolTip(this->meta, this->list.empty());

    this->field->show();
    this->field->setText("");
    this->field->setToolTip(tooltip);
    this->field->setFont(this->tree->font());
    this->field->setFocusPolicy(Qt::StrongFocus);
    this->field->installEventFilter(this);
    this->ResetCompleter();

    this->connect(this->field, SIGNAL(textChanged(const QString &)), this, SLOT(OnTextEdited(const QString &)));
    this->connect(this->field, SIGNAL(returnPressed()), this, SLOT(OnElementAppended()), Qt::QueuedConnection);
    this->connect(this->field, SIGNAL(inputRejected()), this, SLOT(OnElementRejected()), Qt::QueuedConnection);

    this->add_button->show();
    this->add_button->setText("+");
    this->add_button->setFont(this->tree->font());

    this->connect(this->add_button, SIGNAL(pressed()), this, SLOT(OnElementAppended()), Qt::QueuedConnection);

    std::sort(this->data.value.begin(), this->data.value.end());

    this->item->setText(0, (this->meta.label + "  ").c_str());
    this->item->setFont(0, this->tree->font());
    this->item->setToolTip(0, this->meta.description.c_str());
    this->item->setSizeHint(0, QSize(0, ITEM_HEIGHT));
    this->tree->setItemWidget(this->item, 0, this);

    this->Refresh(REFRESH_ENABLE_AND_STATE);
}

void WidgetSettingList::Refresh(RefreshAreas refresh_areas) {
    const bool enabled = ::CheckDependence(this->meta, data_set);

    this->item->setDisabled(!enabled);
    this->setEnabled(enabled);
    this->field->setEnabled(enabled && (!this->meta.list_only || !this->list.empty()));
    this->add_button->setEnabled(enabled && !this->field->text().isEmpty());

    if (this->meta.list_only && this->list.empty()) {
        this->field->hide();
        this->add_button->hide();
    } else {
        this->field->show();
        this->add_button->show();
    }

    if (this->data != this->data_cached) {
        this->data_cached = this->data;

        this->tree->blockSignals(true);

        while (this->item->childCount() > 0) {
            this->item->removeChild(this->item->child(0));
        }

        for (std::size_t i = 0, n = this->data.value.size(); i < n; ++i) {
            this->AddElement(this->data.value[i]);
        }

        this->tree->blockSignals(false);
    }
}

void WidgetSettingList::Resize() {
    const int button_size = MIN_BUTTON_SIZE;

    const QFontMetrics fm = this->fontMetrics();
    const int text_width = HorizontalAdvance(fm, item->text(0));

    this->field->setGeometry(text_width, 0, this->size.width() - button_size - text_width, this->size.height());
    this->add_button->setGeometry(this->size.width() - button_size, 0, button_size, this->size.height());
}

void WidgetSettingList::resizeEvent(QResizeEvent *event) {
    this->size = event->size();
    this->Resize();
}

bool WidgetSettingList::eventFilter(QObject *target, QEvent *event) {
    (void)target;
    if (event->type() == QEvent::Wheel) {
        event->ignore();
        return true;
    }

    return this->field->eventFilter(target, event);
}

void WidgetSettingList::ResetCompleter() {
    if (this->search != nullptr) this->search->deleteLater();

    this->search = new QCompleter(ConvertValues(this->list), this);
    this->search->setCompletionMode(QCompleter::PopupCompletion);
    this->search->setModelSorting(QCompleter::CaseSensitivelySortedModel);
    this->search->setFilterMode(Qt::MatchContains);
    this->search->setCaseSensitivity(Qt::CaseSensitive);
    this->search->setMaxVisibleItems(20);

    this->field->setCompleter(this->search);

    this->connect(this->search, SIGNAL(activated(const QString &)), this, SLOT(OnCompleted(const QString &)), Qt::QueuedConnection);
}

void WidgetSettingList::AddElement(EnabledNumberOrString &element) {
    QTreeWidgetItem *child = new QTreeWidgetItem();
    child->setSizeHint(0, QSize(0, ITEM_HEIGHT));
    this->item->addChild(child);

    WidgetSettingListElement *widget = new WidgetSettingListElement(this->tree, child, this->meta, this->data_set, element);
    this->tree->setItemWidget(child, 0, widget);

    this->connect(widget, SIGNAL(itemChanged()), this, SLOT(OnSettingChanged()));
    this->connect(widget, SIGNAL(itemSelected(const QString &)), this, SLOT(OnElementRemoved(const QString &)));
}

void WidgetSettingList::OnCompleted(const QString &value) {
    (void)value;
    // We can't do this right away, the completer emits it's signal
    // before it's really "complete". If we clear the control too soon
    // it clears the completers value too. This might be a Qt bug, but this
    // works really well as a work-a-round
    OnElementAppended();
}

void WidgetSettingList::OnElementAppended() {
    const QString entry = this->field->text();
    if (entry.isEmpty()) return;

    const std::string string_value = entry.toStdString();
    const bool is_number = IsNumber(string_value);

    EnabledNumberOrString value;
    value.key = is_number ? "" : string_value;
    value.number = is_number ? std::atoi(string_value.c_str()) : 0;
    value.enabled = true;

    if (this->meta.list_only && !IsValueFound(this->meta.list, value)) {
        QMessageBox alert;
        alert.setWindowTitle("Invalid value");
        alert.setText(
            format("'%s' setting doesn't accept '%s' as a value", this->meta.label.c_str(), entry.toStdString().c_str()).c_str());
        alert.setInformativeText("Please select a value from the list.");
        alert.setIcon(QMessageBox::Warning);
        alert.exec();
        return;
    }

    // Add the value if it's not in the list already
    if (IsValueFound(this->data.value, value)) {
        QMessageBox alert;
        alert.setWindowTitle("Duplicated value");
        alert.setText(
            format("'%s' setting already has the value '%s' listed", this->meta.label.c_str(), entry.toStdString().c_str())
                .c_str());
        alert.setIcon(QMessageBox::Warning);
        alert.exec();
        return;
    }

    this->field->setText("");

    this->data.value.push_back(value);
    std::sort(this->data.value.begin(), this->data.value.end());
    ::RemoveValue(this->list, value);

    emit itemChanged();
}

void WidgetSettingList::OnTextEdited(const QString &value) {
    assert(this->add_button);
    assert(this->field);

    // this->tree->blockSignals(true);

    if (value.isEmpty()) {
        this->item->setText(0, (this->meta.label + "  ").c_str());
    } else if (value.size() == 1) {
        this->item->setText(0, "");
    }

    this->Resize();

    this->add_button->setEnabled(!value.isEmpty());

    // this->tree->blockSignals(false);

    // this->field->setFocus();
}

void WidgetSettingList::OnElementRemoved(const QString &element) {
    const std::string string_value = element.toStdString();
    const bool is_number = IsNumber(string_value);

    NumberOrString list_value;
    list_value.key = is_number ? "" : string_value;
    list_value.number = is_number ? std::atoi(string_value.c_str()) : 0;
    this->list.push_back(list_value);

    EnabledNumberOrString data_value;
    data_value.key = list_value.key;
    data_value.number = list_value.number;
    data_value.enabled = true;

    for (auto it = this->data.value.begin(), end = this->data.value.end(); it != end; ++it) {
        if (*it == data_value) {
            this->data.value.erase(it);
            break;
        }
    }

    std::sort(this->data.value.begin(), this->data.value.end());
}

void WidgetSettingList::OnElementRejected() { this->OnTextEdited(""); }

void WidgetSettingList::OnSettingChanged() { emit itemChanged(); }

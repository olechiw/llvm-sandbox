//
// Created by jolechiw on 7/9/23.
//

#include "ComboBox.h"

void ComboBox::render() {
    if (ImGui::BeginCombo(_name, _currentOption)) {
        for (auto &option: _options) {
            bool isSelected = _currentOption == option;
            if (ImGui::Selectable(option, isSelected)) {
                _currentOption = option;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

const char *ComboBox::getCurrentOption() {
    return _currentOption;
}

void ComboBox::setCurrentOption(const char *option) {
    _currentOption = option;
}

ComboBox::ComboBox(const char *name, const std::vector<const char *> &options) : _options{options}, _name{name},
                                                                                 _currentOption{options.front()} {
}


//
// Created by jolechiw on 7/9/23.
//

#include "ComboBox.h"

void ComboBox::render() {
    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s", _label);
    ImGui::SameLine();
    float width = 0;
    for (const auto &option: _options) {
        width = std::max(width, ImGui::CalcTextSize(option).x);
    }
    constexpr float additionalPadding = 5.f;
    width += ImGui::GetStyle().FramePadding.x * 2.0f + ImGui::GetTextLineHeightWithSpacing() + additionalPadding;
    ImGui::SetNextItemWidth(width);
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

ComboBox::ComboBox(const char *name, const char *label, const std::vector<const char *> &options) : _options{options},
                                                                                                    _label{label},
                                                                                                    _name{name},
                                                                                                    _currentOption{
                                                                                                            options.front()} {
}


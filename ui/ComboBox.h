//
// Created by jolechiw on 7/9/23.
//

#ifndef TESTPROJECT_COMBOBOX_H
#define TESTPROJECT_COMBOBOX_H

#include <vector>
#include <string>

#include "imgui.h"

class ComboBox {
public:
    ComboBox(const char *name, const char *label, const std::vector<const char *> &options);

    void render();

    const char *getCurrentOption();

    void setCurrentOption(const char *option);

private:
    std::vector<const char *> _options;
    const char *_name;
    const char *_currentOption{nullptr};
    const char *_label;
};


#endif //TESTPROJECT_COMBOBOX_H

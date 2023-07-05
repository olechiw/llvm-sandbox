//
// Created by jolechiw on 6/29/23.
//

#ifndef TESTPROJECT_DIAGNOSTICSVIEW_H
#define TESTPROJECT_DIAGNOSTICSVIEW_H

#include <TextEditor.h>

#include "../model/Diagnostics.h"

class DiagnosticsView {
public:
    explicit DiagnosticsView(Diagnostics &diagnostics);

    void render();
private:
    Diagnostics &_diagnostics;
    TextEditor _textEditor {};
};


#endif //TESTPROJECT_DIAGNOSTICSVIEW_H

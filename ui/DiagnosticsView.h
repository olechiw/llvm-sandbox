//
// Created by jolechiw on 6/29/23.
//

#ifndef TESTPROJECT_DIAGNOSTICSVIEW_H
#define TESTPROJECT_DIAGNOSTICSVIEW_H

#include <TextEditor.h>

#include "../model/Context.h"

class DiagnosticsView {
public:
    explicit DiagnosticsView(Context &context);

    void render();
private:
    Context &_context;
    TextEditor _textEditor {};
};


#endif //TESTPROJECT_DIAGNOSTICSVIEW_H

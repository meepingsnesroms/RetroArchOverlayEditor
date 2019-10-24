#ifndef ADVANCEDEDITOR_H
#define ADVANCEDEDITOR_H

#include <QDialog>

#include "ui_advancededitor.h"

namespace Ui{
class AdvancedEditor;
}

class AdvancedEditor : public QDialog{
   Q_OBJECT

public:
   explicit AdvancedEditor(QWidget* parent = nullptr);
   ~AdvancedEditor();

public:
   //this is meant to be public and fully controlled by the main window
   Ui::AdvancedEditor* ui;
};

#endif

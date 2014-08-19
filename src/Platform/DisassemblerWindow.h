#ifndef DISASSEMBLERWINDOW_H
#define DISASSEMBLERWINDOW_H

#include <QDialog>

namespace Ui {
    class MainWindow;
    class DisassemblerWindow;
}

class MainWindow;

class DisassemblerWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DisassemblerWindow(QWidget* parent = 0);
    ~DisassemblerWindow();

    void UpdateLabelData();

private:
    MainWindow* _mainWindow;
    Ui::DisassemblerWindow* ui;
};

#endif // MAINWINDOW_H

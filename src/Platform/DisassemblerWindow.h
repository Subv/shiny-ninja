#ifndef DISASSEMBLERWINDOW_H
#define DISASSEMBLERWINDOW_H

#include <QDialog>
#include <array>

namespace Ui {
    class MainWindow;
    class DisassemblerWindow;
}

class MainWindow;
class QLabel;

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
    std::array<QLabel*, 16> _registers;
    std::array<QLabel*, 4> _flags;
};

#endif // MAINWINDOW_H

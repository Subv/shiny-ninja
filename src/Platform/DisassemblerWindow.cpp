#include "MainWindow.h"
#include "CPU/CPU.hpp"
#include "DisassemblerWindow.h"
#include <QLabel>

#include "ui_DisassemblerWindow.h"

DisassemblerWindow::DisassemblerWindow(QWidget* parent) : QDialog(parent),
ui(new Ui::DisassemblerWindow),
_mainWindow(static_cast<MainWindow*>(parent))
{
    ui->setupUi(this);
    UpdateLabelData();
}

void DisassemblerWindow::UpdateLabelData()
{
    if (!_mainWindow->GetCPU())
        return;
    for (int i = 0; i <= 15; ++i)
    {
        auto label = findChild<QLabel*>(QString::fromStdString("label_r" + std::to_string(i)));
        label->setText(QString::number(_mainWindow->GetCPU()->GetRegister(i), 16));
    }
}

DisassemblerWindow::~DisassemblerWindow()
{
    delete ui;
}

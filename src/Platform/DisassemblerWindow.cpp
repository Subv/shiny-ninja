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

    for (int i = 0; i <= 15; ++i)
        _registers[i] = findChild<QLabel*>(QString::fromStdString("label_r" + std::to_string(i)));

    _flags[0] = findChild<QLabel*>("V");
    _flags[1] = findChild<QLabel*>("C");
    _flags[2] = findChild<QLabel*>("Z");
    _flags[3] = findChild<QLabel*>("N");

    UpdateLabelData();
}

void DisassemblerWindow::UpdateLabelData()
{
    if (!_mainWindow->GetCPU())
        return;

    for (int i = 0; i <= 15; ++i)
        _registers[i]->setText(QString::number(_mainWindow->GetCPU()->GetRegister(i), 16));

    _flags[0]->setText(QString::number(_mainWindow->GetCPU()->GetCurrentStatusFlags().V));
    _flags[1]->setText(QString::number(_mainWindow->GetCPU()->GetCurrentStatusFlags().C));
    _flags[2]->setText(QString::number(_mainWindow->GetCPU()->GetCurrentStatusFlags().Z));
    _flags[3]->setText(QString::number(_mainWindow->GetCPU()->GetCurrentStatusFlags().N));
}

DisassemblerWindow::~DisassemblerWindow()
{
    delete ui;
}

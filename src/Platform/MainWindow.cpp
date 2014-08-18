#include "MainWindow.h"

#include "CPU/CPU.hpp"
#include "Common/Utilities.hpp"

#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>

#include "ui_MainWindow.h"

#include <iostream>
#include <thread>
#include <chrono>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Run()
{

}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Open ROM File", "",
                                                    tr("GBA Files (*.gba);;"
                                                       "All Files (*.*)"));

    if (!fileName.count())
        return;

    FILE* rom = fopen(fileName.toUtf8().constData(), "rb");

    if (!rom)
    {
        QMessageBox::critical(this, "Error loading ROM", "Could not load specified ROM file", QMessageBox::Ok);
        return;
    }

    int read = fread(&_header, sizeof(GBAHeader), 1, rom);

    if (!read)
    {
        QMessageBox::critical(this, "Error loading ROM", "Could not read ROM header.", QMessageBox::Ok);
        fclose(rom);
        return;
    }

    if (!_header.VerifyHeader())
    {
        QMessageBox::critical(this, "Error loading ROM", "The specified file is not a valid GBA ROM.", QMessageBox::Ok);
        fclose(rom);
        return;
    }

    _cpu = std::shared_ptr<CPU>(new CPU(CPUExecutionMode::Interpreter));
    RegisterCPUCallbacks();

    _cpu->LoadROM(_header, rom);

    fclose(rom);

    // Run the CPU in a different thread
    _cpuThread = std::thread([&] {
        _cpu->Run();
    });
}

void MainWindow::restart()
{
    _cpu->Reset();
    _cpuThread.join();

    _cpuThread = std::thread([&] {
        _cpu->Run();
    });
}

void MainWindow::RegisterCPUCallbacks()
{
    _cpu->RegisterInstructionCallback(InstructionCallbackTypes::InstructionExecuted, [&](std::shared_ptr<Instruction> instruction)
    {
        QString message = QString::fromUtf8(("Set: " + std::string(instruction->GetInstructionSet() == InstructionSet::ARM ? "ARM" : "Thumb") + ". Instruction: " + instruction->ToString()).c_str());
        findChild<QLabel*>("label")->setText(message);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    });
}

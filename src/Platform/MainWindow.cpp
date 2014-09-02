#include "MainWindow.h"

#include "CPU/CPU.hpp"
#include "Common/Utilities.hpp"

#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QCheckBox>

#include "DisassemblerWindow.h"
#include "ui_MainWindow.h"

#include <iostream>
#include <thread>
#include <chrono>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _disasmWindow(nullptr),
    instructionDelay(50)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    _cpu->Stop();
    _cpuThread.join();
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

    FILE* bios = fopen("./gba_bios.bin", "rb");
    if (!bios)
    {
        std::cout << "Could not load the GBA Bios." << std::endl;
        return;
    }

    _cpu = std::shared_ptr<CPU>(new CPU(CPUExecutionMode::Interpreter));
    RegisterCPUCallbacks();

    _cpu->LoadROM(_header, rom, bios);

    fclose(bios);
    fclose(rom);

    auto checkbox = findChild<QCheckBox*>("runOnLoad");

    if (checkbox->isChecked())
    {
        // Run the CPU in a different thread
        _cpuThread = std::thread([&] {
            _cpu->Run();
        });
    }
}

void MainWindow::restart()
{
    if (!_cpu)
        return;

    _cpu->Stop();
    _cpuThread.join();
    _cpu->Reset();
}

void MainWindow::RegisterCPUCallbacks()
{
    if (!_cpu)
        return;

    _cpu->RegisterInstructionCallback(InstructionCallbackTypes::InstructionExecuted, [&](std::shared_ptr<Instruction> instruction)
    {
        QString message = QString::fromUtf8(("Set: " + std::string(instruction->GetInstructionSet() == InstructionSet::ARM ? "ARM" : "Thumb") + ". Instruction: " + instruction->ToString()).c_str());
        emit instructionExecuted(message);

        if (_cpu->GetRegister(PC) == 0x8000166 || _cpu->GetRegister(PC) == 0x8000170)
            _cpu->Stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(instructionDelay));
    });
}

void MainWindow::openDisassembler()
{
    if (_disasmWindow)
    {
        _disasmWindow->show();
        return;
    }
    _disasmWindow = new DisassemblerWindow(this);
    _disasmWindow->show();
}

void MainWindow::pause()
{
    if (_cpu)
    {
        _cpu->Stop();
        _cpuThread.join();
    }
}

void MainWindow::step()
{
    if (_cpu)
        _cpu->Step();
}

void MainWindow::resume()
{
    if (_cpu)
    {
        _cpuThread = std::thread([&] {
            _cpu->Run();
        });
    }
}

void MainWindow::updateInstDelay()
{
    auto textbox = findChild<QLineEdit*>("instDelay");
    instructionDelay = std::max(1, textbox->text().toInt());
}

void MainWindow::onInstructionExecuted(QString message)
{
    findChild<QLabel*>("label")->setText(message);
    if (_disasmWindow)
        _disasmWindow->UpdateLabelData();
}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <thread>

#include "Common/GBA.hpp"

class CPU;

namespace Ui {
class MainWindow;
}

class DisassemblerWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void Run();
    void RegisterCPUCallbacks();
    std::shared_ptr<CPU> GetCPU() { return _cpu; }

private slots:
    void open();
    void restart();
    void pause();
    void step();
    void openDisassembler();
    
private:
    DisassemblerWindow* _disasmWindow;
    std::shared_ptr<CPU> _cpu;
    GBAHeader _header;
    std::thread _cpuThread;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

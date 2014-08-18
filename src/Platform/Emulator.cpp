#include <QApplication>
#include "NoGUI.hpp"
#include "MainWindow.h"

int main(int argc, char* argv[])
{
    // Start the application without a GUI
    if (argc > 1 && !strcmp(argv[1], "--no-gui"))
    {
        NoGUI noGUI(argc, argv);
        return noGUI.Run();
    }

    // Start the Qt GUI
    QApplication app(argc, argv);
    MainWindow mw;
    mw.show();
    return app.exec();
}

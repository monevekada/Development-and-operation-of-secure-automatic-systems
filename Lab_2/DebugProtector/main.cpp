#include "mainwindow.h"

#include <QApplication>
#include <Windows.h>
#include <QDebug>

int main(int argc, char *argv[])
{
    // Получить из аргументов PID процесса, который надо защищать
    DWORD pid = (DWORD) atoi(argv[1]);

    bool isAttacked = DebugActiveProcess(pid); // Подключаемся к нему как отладчик

    if(!isAttacked) { // Проверяем, удалось ли подключиться
        DWORD lastError = GetLastError();
        qDebug() << "GetLastError()" << lastError;
        return lastError;
    }

    DEBUG_EVENT DbgEvent;

    while(1) { //
        WaitForDebugEvent(&DbgEvent, INFINITE);
        ContinueDebugEvent(DbgEvent.dwProcessId, DbgEvent.dwThreadId, DBG_CONTINUE);
    }

//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();
//    return a.exec();
}

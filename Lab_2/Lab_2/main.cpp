#include <QApplication>
#include <QMainWindow>
#include <QQmlApplicationEngine>
#include <QGuiApplication>
#include <QCoreApplication>
#include <QMessageBox>
#include <QDebug>
#include <QObject>
#include <QQmlContext>
#include <QCryptographicHash>
#include <QByteArray>
#include <QProcess>


#include <Windows.h> // для корректной сборки с debugapi.h
#include <debugapi.h>
#include <intrin.h>
#include <iostream>
#include <atlstr.h>
#include <string.h>

#include "cryptocontroller.h"

typedef unsigned long long QWORD;


int main(int argc, char *argv[])
{    
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    cryptoController crp;
    QQmlContext *context = engine.rootContext();
    context->setContextProperty("cryptoController", &crp);



    //// БЛОК ЗАЩИТЫ ОТ ОТЛАДКИ МЕТОДОМ САМООТЛАДКИ
    ///
    QProcess *satelliteProcess = new QProcess();
    int pid = QApplication::applicationPid();
    qDebug() << "pid = " << pid;
    QStringList arguments = {QString::number(pid)};
    qDebug() << "arguments = " << arguments;
    satelliteProcess->start("DebugProtector.exe", arguments);
    bool ProtectorStarted = satelliteProcess->waitForStarted(1000);
    qDebug() << "ProtectorStarted = " << ProtectorStarted;


    //// БЛОК САМОПРОВЕРКИ ХЭШ-СУММЫ
    ///
        // Чтобы определить начальный адрес, с которого приложение размещается в виртуальной памяти,
        // вызывается WinAPI-функция GetModuleHandle
        QWORD moduleBase = (QWORD)GetModuleHandle(NULL);
        QWORD text_segment_start = moduleBase + 0x1000; // Сегмент .text располагается с отступом 0x1000
        qDebug() << "text_segment_start = " << Qt::hex << text_segment_start;

        // Определить длину сегмента из заголовка
        PIMAGE_DOS_HEADER pIHD = reinterpret_cast<PIMAGE_DOS_HEADER>(moduleBase);
        PIMAGE_NT_HEADERS pINH = reinterpret_cast<PIMAGE_NT_HEADERS>(moduleBase + pIHD->e_lfanew);

        // Размер сегмента .text
        QWORD size_of_text = pINH->OptionalHeader.SizeOfCode;

        QByteArray text_segment_contents = QByteArray((char*)text_segment_start, size_of_text); // Помещаем содержимое сегмента .text в массив char QByteArray
        QByteArray current_hash = QCryptographicHash::hash(text_segment_contents, QCryptographicHash::Sha256); // Получаем хэш сегмента .text
        QByteArray current_hash_base64 = current_hash.toBase64(); // Переводим хэш в формат Base64, т.к. некоторые символы могут быть нечитыаемы в исходном хэше

        // Задаем константу, в которой будет храниться эталонный хэш
        const QByteArray hash0_base64 = QByteArray("ObODmyZ9xS1b8V23lbwXEbZCcm/MGlTWG14S31dxLtc=");

        // Для проверки выводим все данные в отладку
        qDebug() << "text_segment_start = " << Qt::hex << text_segment_start;
        qDebug() << "size_of_text = " << size_of_text;
        qDebug() << "text_segment_contents = " << Qt::hex << text_segment_contents.remove(100, text_segment_contents.length()); //.first(100);
        qDebug() << "current_hash_base64 = " << current_hash_base64;

        bool checkresult = (current_hash_base64==hash0_base64); // Проверка эталлоного хэша с фактическим
        qDebug() << "checkresult = " << checkresult; // Вывод результата проверки

        if (!checkresult) { // Если хэш приложения не совпадает с эталонным,
             qDebug() << "Attention !!! Modification of an App !!! Teminating the App !!!";
             return -1; // приложение закрывается с ошибкой
        }

    //// БЛОК ПРОСТЕЙШЕГО ОБНАРУЖЕНИЯ ОТЛАДЧИКА
    ///
//    bool debuggerFound = IsDebuggerPresent();
//    if (debuggerFound) {
//        qDebug() << "Debugger is found !!! Teminating the App !!!";
//        std::cout << "Debugger is found !!! Teminating the App !!!" << std::endl;
//        return -1;
//    }

    engine.load(url);
    return app.exec();
}

#include "cryptocontroller.h"
#include <QString>
#include <openssl/evp.h>
#include <QFile>
#include <QIODevice>
#include <QObject>
#include <openssl/conf.h>
#include <QQmlContext>
#include <QDebug>
#include <QClipboard>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QByteArray>
#include <QProcess>
#include <QClipboard>
#include <atlstr.h>
#include <Windows.h>
#include <intrin.h>
#include <iostream>
#include <string.h>

using namespace std;

cryptoController::cryptoController(QObject *parent) : QObject(parent)
{

}

void cryptoController::text_to_clipboard(QString text) {
    if (OpenClipboard(0))
    {
        EmptyClipboard();
        HGLOBAL hBuff = (char *) GlobalAlloc(GMEM_FIXED, text.length() + 1);
        memcpy(hBuff, text.toStdString().c_str(), text.length() + 1);
        SetClipboardData(CF_TEXT, hBuff);
        CloseClipboard();
    }
}

void cryptoController::decrypt_login_or_password(QString text, QString password) {

    EVP_CIPHER_CTX *ctx;
    if(!(ctx = EVP_CIPHER_CTX_new())){
        return;
    }

    QString key = password + ((QString)"0").repeated(32 - password.length());

    iv = (unsigned char *) key.data();

    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cfb(), NULL, (unsigned char *) key.data(), iv))
    {
        return;
    }

    unsigned char ciphertext[256] = {0};
    unsigned char plaintext[256] = {0};
    int len = 0, plaintext_len = 256;
    QString db_decrypted = "";

    QByteArray line = QByteArray::fromBase64(text.toUtf8());
    int s = line.size();
    if (s<256)
        plaintext_len=s;
    memcpy(plaintext, line.toStdString().c_str(), plaintext_len);

    QByteArray ciphertext_qbyte;
    while(s > 0){
        if(1 != EVP_DecryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        {
            return;
        }

        ciphertext_qbyte = QByteArray(((char *) ciphertext), len);

        line.remove(0, 256);
        s -= 256;
        if (s < 0)
            break;
        if (s < 256)
            plaintext_len=s;
        memcpy(plaintext, line.toStdString().c_str(), plaintext_len);
    }

    EVP_CIPHER_CTX_free(ctx);

    text_to_clipboard(ciphertext_qbyte);
}

bool cryptoController::check_password(QString password) {

    //encrypt_db_file(password, -1);

    EVP_CIPHER_CTX *ctx;
    if(!(ctx = EVP_CIPHER_CTX_new())){
        return 0;
    }

    QString key = password + ((QString)"0").repeated(32 - password.length());

    iv = (unsigned char*) key.data();

    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cfb(), NULL, (unsigned char*) key.data(), iv))
    {
        return false;
    }

    unsigned char ciphertext[256] = {0};
    unsigned char plaintext[256] = {0};
    int len = 0, plaintext_len = password.length();

    memcpy(plaintext, password.toStdString().c_str(), password.size());

    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
    {
        return false;
    }

    QString password_cipher = (char *)ciphertext;
    QByteArray password_cipher_qbyte = (char*) ciphertext;

    char buffer[256] = {0};
    QFile source_file(":/password_crypt.txt");
    source_file.open(QIODevice::ReadOnly);
    source_file.read(buffer, 256);
    source_file.close();

    EVP_CIPHER_CTX_free(ctx);

    return QString(buffer) == password_cipher_qbyte.toBase64();
}


void cryptoController::decrypt_db_file(QString password, int is_password)
{
    EVP_CIPHER_CTX *ctx;
    if(!(ctx = EVP_CIPHER_CTX_new())){
        return;
    }

    QString key = password + ((QString)"0").repeated(32 - password.length());

    iv = (unsigned char*) key.data();

    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cfb(), NULL, (unsigned char*) key.data(), iv))
    {
        return;
    }

    unsigned char ciphertext[256] = {0};
    unsigned char plaintext[256] = {0};
    int len = 0, plaintext_len = 0;
    QString db_decrypted = "";

    QFile source_file(":/db_crypt.json");
    bool is_opened = source_file.open(QIODevice::ReadOnly);
    if(!is_opened) {
        return;
    }

    plaintext_len = source_file.read((char *)plaintext, 256);

    while(plaintext_len > 0){
        if(1 != EVP_DecryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        {
            return;
        }

        QByteArray ciphertext_qbyte = QByteArray(((char *) ciphertext), len);
        db_decrypted += ciphertext_qbyte;

        plaintext_len = source_file.read((char *)plaintext, 256);
    }

    EVP_CIPHER_CTX_free(ctx);

    if(is_password == -1)
        emit sendDbToQml(db_decrypted);
}

void cryptoController::encrypt_db_file(QString password, int is_password)
{
//    EVP_CIPHER_CTX *ctx;
//    if(!(ctx = EVP_CIPHER_CTX_new())){
//        return;
//    }

//    QString key = password + ((QString)"0").repeated(32 - password.length());

//    iv = (unsigned char*) key.data();

//    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cfb(), NULL, (unsigned char*) key.data(), iv))
//    {
//        return;
//    }

//    unsigned char ciphertext[256] = {0};
//    unsigned char plaintext[256] = {0};
//    int len = 0, plaintext_len = 0;
//    QString db_decrypted = "";

//    QFile source_file(":/db.json");
//    bool is_opened = source_file.open(QIODevice::ReadOnly);
//    if(!is_opened) {
//        return;
//    }

//    plaintext_len = source_file.read((char *)plaintext, 256);


//    QFile file("F:/Polytech/3-6/Razrabotka/Qt_project/Lab_2/db_crypt.json");
//    file.open(QIODevice::WriteOnly);

//    while(plaintext_len > 0){
//        if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
//        {
//            return;
//        }

//        QByteArray ciphertext_qbyte = QByteArray(((char *) ciphertext), len);
//        db_decrypted += ciphertext_qbyte;

//        file.write((char *)ciphertext, len);
//        plaintext_len = source_file.read((char *)plaintext, 256);
//    }


//    EVP_CIPHER_CTX_free(ctx);

//    if(is_password == -1)
//        emit sendDbToQml(db_decrypted);


    // Шифрование пароля/логина
//    EVP_CIPHER_CTX *ctx;
//    if(!(ctx = EVP_CIPHER_CTX_new())){ //EVP_CIPHER_CTX_new() creates a cipher context.
//        return;
//    }

//    QString key = password + ((QString)"0").repeated(32 - password.length());

//    iv = (unsigned char *) key.data();

//    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cfb(), NULL, (unsigned char*) key.data(), iv))
//    {
//        return;
//    }

//    unsigned char ciphertext[256] = {0};
//    unsigned char plaintext[256] = {0};
//    int len = 0, plaintext_len = 256;
//    QString db_decrypted = "";

//    QFile out("F:/Polytech/3-6/Razrabotka/Qt_project/Lab_2/out.txt");
//    bool is_opened = out.open(QIODevice::ReadOnly);
//    if(!is_opened) {
//        return;
//    }

//    plaintext_len = out.read((char*)plaintext, 256);

//    QFile in("F:/Polytech/3-6/Razrabotka/Qt_project/Lab_2/in.txt");
//    in.open(QIODevice::WriteOnly);
//    QByteArray ciphertext_qbyte;
//    while(plaintext_len > 0){
//        if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
//        {
//            return;
//        }

//        ciphertext_qbyte = QByteArray(((char *) ciphertext), len);
//        in.write(ciphertext_qbyte.toBase64());
//        plaintext_len = out.read((char *)plaintext, 256);
//    }

//    in.close();
//    out.close();

//    EVP_CIPHER_CTX_free(ctx);

}


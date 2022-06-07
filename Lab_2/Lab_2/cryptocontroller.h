#ifndef CRYPTOCONTROLLER_H
#define CRYPTOCONTROLLER_H

#include <QObject>
#include <QString>

class cryptoController : public QObject
{
    Q_OBJECT
public:
    explicit cryptoController(QObject *parent = nullptr);
    ~cryptoController() {};

public slots:
    bool check_password(QString password);
    void decrypt_db_file(QString password, int is_password=-1);
    void encrypt_db_file(QString password, int is_password=-1);
    void text_to_clipboard(QString text);
    void decrypt_login_or_password(QString text, QString password);

private:
    unsigned char * iv = (unsigned char *)("00000000000000000000000000000000");

signals:
    void sendDbToQml(QString db_decrypted);
    void sendMessageToQml(QString message);

};

#endif // CRYPTOCONTROLLER_H

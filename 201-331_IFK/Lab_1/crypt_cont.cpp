#include "crypt_cont.h"
#include <openssl/evp.h>


crypt_cont::crypt_cont()
{

}

crypt_cont::~crypt_cont() {

}

int crypt_cont::encrypt_file(QString file, unsigned char *key) {

    // Есть два файла, один с открытыми данными, второй с полностью закрытыми.

    QFile initial_file; // Файл с полностью открытыми данными
    initial_file.setFileName(open_file_name);

    initial_file.open(QIODevice::ReadOnly | QIODevice::Text);

    if (!initial_file.isOpen()) {
        qDebug() << "Ошибка в открытии исходного файла";
        return 0;
    }

    QString initial_data = initial_file.readAll();

    initial_file.close();

    //qDebug() << initial_data;


    // Создание JSON структуры
    doc = QJsonDocument::fromJson(QByteArray(initial_data.toUtf8()), &docError);

    json = doc.object();

    credentials = json.value("credentials");


    //qDebug() << credentials;

    //JSON для хранения названия сайта и зашифрованной связки логин + пароль
    QJsonObject json_new;
    QJsonArray credentials_new;
    QJsonObject aut_data;

    for (int i = 0; credentials[i].isObject(); i++) {
        QJsonObject obj = credentials[i].toObject();//Создаем объект из значений в массиве credentials
        //(char)123
        //QString log_pass_str = QString("{") + "\"login\":\"" +  obj["login"].toString() + '"' + ',' + "\"password\":\"" + obj["password"].toString() + '"' + QString("}"); //Собираем логин и пароль в одну строчку

        QByteArray chipr_log_pass_bit = chipr_log_and_pass(obj["login"].toString(), obj["password"].toString(), key); // Результат переводим в байтовое значение

        aut_data.insert("site",obj["site"].toString());
        aut_data.insert("data",chipr_log_pass_bit.data());

        credentials_new.push_back(aut_data);

    }

    json_new.insert("credentials", credentials_new);
    QJsonDocument chip_log_pass(json_new); // JSON документ с открытым сайтом и шифрованными логином и паролем


    // Шифрование всего файла

    unsigned char *chipr_full_file = new unsigned char[chip_log_pass.toJson().length() * 2];
    int chipr_full_file_len = encrypt((unsigned char *)chip_log_pass.toJson().data(), chip_log_pass.toJson().length(), key, iv, chipr_full_file);
    QByteArray chipr_full_file_bit =  QByteArray::fromRawData((const char*)chipr_full_file, chipr_full_file_len).toBase64();

    qDebug() << "Полностью зашифрованный файл: " << chipr_full_file_bit;

    delete [] chipr_full_file;



    //qDebug() << key;
    QFile encrypt_file(file);

    encrypt_file.open(QIODevice::WriteOnly | QIODevice::Text);

    encrypt_file.write(chipr_full_file_bit);

    encrypt_file.close();

    return 0;
}

QString crypt_cont::decrypt_file(QString file, unsigned char *key) {

    QFile encrypted_file;

    encrypted_file.setFileName(file);

    encrypted_file.open(QFile::ReadOnly);

    if (!encrypted_file.isOpen()) {
        qDebug() << "Ошибка в открытии файла";
        return 0;
    }

    QByteArray file_data_encode = QByteArray::fromBase64(encrypted_file.readAll());// Считываем из файла байтовые данные
    QBuffer chiper_buffer(&file_data_encode); // Создаем буфер из считанных байтов
    chiper_buffer.open(QBuffer::ReadOnly); // Открываем буфер

    QString finaly_decrypt_text; // Строка в которой будут хранится расшифрованные данные

    //qDebug() << key;

    EVP_CIPHER_CTX *ctx; // Создаем структуру

    if(!(ctx = EVP_CIPHER_CTX_new()))
        crypt_error();

    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) //Инициализация структуры
        crypt_error();

    int read_buffer = 0; // количество считанных из буфера символов
    int meter_seek = 0; // счетчик сдвига курсора буфера
    int decryptedtext_len = 0; // длинна расшифрованного текста

    while(true) {
        char ciphertext[256] = {0}; //
        unsigned char decryptedtext[512] = {0};

        chiper_buffer.seek(meter_seek);// Сдвиг курсора буфера

        read_buffer = chiper_buffer.read(ciphertext,256); // считываем из буфера порции по 256 символов

        //Расшифровка
        if(1 != EVP_DecryptUpdate(ctx, decryptedtext, &decryptedtext_len, (unsigned char*)ciphertext, read_buffer))
            crypt_error();

        QString current_block; // Строка с текущими расшифрованными символами

        if (read_buffer < 256) {
            int temp;
            if(1 != EVP_DecryptFinal_ex(ctx, decryptedtext + decryptedtext_len, &temp)) // ?????????????????????
                crypt_error();

            // Вывод расшифрованных символов в строку
            for (int var = 0; var < decryptedtext_len + temp; var++) {

                current_block.append((char)decryptedtext[var]); // из массива чаров в стринг

            }

            EVP_CIPHER_CTX_free(ctx);// Очищение структуры

            decryptedtext[decryptedtext_len] = '\0';

            finaly_decrypt_text.append(current_block); // Добавление текущего блока в общий

            break;
        }


        // Вывод расшифрованных символов в строку
        for (int var = 0; var < decryptedtext_len; var++) {

            current_block.append((char)decryptedtext[var]); // из массива чаров в стринг

        }

        meter_seek += 256; // Сдвиг курсора

        finaly_decrypt_text.append(current_block); // Добавление текущего блока в общий

    }
    //qDebug() << finaly_decrypt_text;

    return finaly_decrypt_text;
}


int crypt_cont::encrypt(unsigned char *text, int text_len, unsigned char *key,
                    unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;


    if(!(ctx = EVP_CIPHER_CTX_new()))
        crypt_error();


    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        crypt_error();


    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, text, text_len))
        crypt_error();
    ciphertext_len = len;

    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        crypt_error();
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}


int crypt_cont::decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
                    unsigned char *iv, unsigned char *decryptext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int decryptext_len;

    if(!(ctx = EVP_CIPHER_CTX_new()))
        crypt_error();


    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        crypt_error();


    if(1 != EVP_DecryptUpdate(ctx, decryptext, &len, ciphertext, ciphertext_len))
        crypt_error();
    decryptext_len = len;


    if(1 != EVP_DecryptFinal_ex(ctx, decryptext + len, &len))
        crypt_error();
    decryptext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return decryptext_len;
}

QByteArray crypt_cont::chipr_log_and_pass(QString log, QString pass, unsigned char *key) {

    QString log_pass_str = QString("{") + "\"login\":\"" +  log + '"' + ',' + "\"password\":\"" + pass + '"' + QString("}");

    QByteArray log_pass_bit = log_pass_str.toUtf8(); // Переводим в байтовое значение

    unsigned char chipr[512];

    int chipr_log_pas_len = encrypt((unsigned char *)log_pass_bit.data(), log_pass_bit.length(), key, iv, chipr); // Шифруем строчку с логином и паролем
    QByteArray chipr_log_pass_bit =  QByteArray::fromRawData((const char*)chipr, chipr_log_pas_len).toBase64(); // Результат переводим в байтовое значение

    qDebug() << "chipr_log_pass_bit = " + chipr_log_pass_bit;

    return chipr_log_pass_bit;

}
int crypt_cont::crypt_error(void)
{
    return 1;
}

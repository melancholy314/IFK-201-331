#include "windows.h"
#include "mainwindow.h"
#include "crypt_cont.h"
#include <QApplication>
#include "QByteArray"
#include "QMessageBox"


#define CHECK_SUM_ON
#define DEBUG_PROTECT_ON


int check_pin_fun() {
    return 0;
}


int main(int argc, char *argv[])
{

#ifdef CHECK_SUM_ON
    // Определяем сегмент .text в виртуальной памяти
    unsigned long long moduleBase = (unsigned long long)GetModuleHandle(NULL); // определение начального адреса
    unsigned long long text_segment_start = moduleBase + 0x1000; // адрес сегмента .text
    // Определяем длинну этого сегмента
    PIMAGE_DOS_HEADER pIDH = reinterpret_cast<PIMAGE_DOS_HEADER>(moduleBase); //Определяется адресс DOS заголовка
    PIMAGE_NT_HEADERS pINH = reinterpret_cast<PIMAGE_NT_HEADERS>(moduleBase + pIDH->e_lfanew); //e_lfanew хранит адресс нового заголовка. По нему находим NT заголовок
    unsigned long long size_of_text = pINH->OptionalHeader.SizeOfCode; // В заголовке OptionalHeader находится размер .text (SizeOfCode). Извлекаем его
    // Подсчет хэша
    QByteArray text_segment_contents = QByteArray((char * )text_segment_start, size_of_text); // Создаем массив байтов из .text
    QByteArray current_hash = QCryptographicHash::hash(text_segment_contents, QCryptographicHash::Sha256);// Хэшируем по Sha256
    QByteArray current_hash_base64 = current_hash.toBase64(); //Переводим в Base64
    // Сравнение хэша

    qDebug() << current_hash_base64;
    const QByteArray ref_hash_base64 = QByteArray("M9HhJhLqaFGZJjckdra0GBYAwv3D5B5qI95rqpsNxgo=");
    bool result_checksum = (current_hash_base64==ref_hash_base64);
#else
    bool result_checksum = true;
#endif
    //Проверка на отладку
#ifdef DEBUG_PROTECT_ON
    if (IsDebuggerPresent()) {
        QMessageBox::critical(NULL,QObject::tr("Ошибка"),QObject::tr("Запущен отладчик"));
        return 0;
    }
#endif


    QApplication a(argc, argv);

    //Проверка на чексуммы
    if(!result_checksum) {
        QMessageBox::critical(NULL,QObject::tr("Ошибка"),QObject::tr("Хэш-код приложения изменен"));
        return 0;
    }

    MainWindow window;
    window.show_pin_check_window();//Вызов окна проверки пин кода


    return a.exec();

}


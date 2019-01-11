#include <cmath>
#include "ui_widget.h"
#include "widget.h"
#include "windows.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this] ()
        {
            QString diskInfo = "<b><u>Тип диску:</u></b> ";
            WCHAR diskName[5] = {0};
            this->ui->comboBox->currentText().toWCharArray(diskName);
            switch (GetDriveTypeW(diskName))
            {
                case DRIVE_CDROM: diskInfo += "CD-ROM"; break;
                case DRIVE_UNKNOWN: diskInfo += "Невідомий"; break;
                case DRIVE_NO_ROOT_DIR: diskInfo += "Диск було від'єднано"; break;
                case DRIVE_REMOVABLE: diskInfo += " Знімний диск"; break;
                case DRIVE_FIXED: diskInfo += " Незнімний диск"; break;
                case DRIVE_REMOTE: diskInfo += " Мережевий диск"; break;
                case DRIVE_RAMDISK: diskInfo += " RAM-диск"; break;
            }

            if (GetDriveType(diskName)!=DRIVE_NO_ROOT_DIR)
            {
                ULARGE_INTEGER totalBytes, freeBytes;
                GetDiskFreeSpaceExW(diskName, nullptr, &totalBytes, &freeBytes);
                diskInfo += "<br /><b><u>Загальний обсяг:</u></b> ";
                diskInfo += QString::number(totalBytes.QuadPart) + " байт (" + QString::number(static_cast<double>(totalBytes.QuadPart) / 1024 / 1024 / 1024) + " ГБ)";
                diskInfo += "<br /><b><u>Вільний обсяг:</u></b> ";
                diskInfo += QString::number(freeBytes.QuadPart) + " байт (" + QString::number(static_cast<double>(freeBytes.QuadPart) / 1024 / 1024 / 1024) + " ГБ)";

                DWORD sectorsPerCluster, bytesPerSector;
                GetDiskFreeSpaceW(diskName, &sectorsPerCluster, &bytesPerSector, nullptr, nullptr);
                diskInfo += "<br /><b><u>Кількість секторів у кластері:</u></b> ";
                diskInfo += QString::number(sectorsPerCluster);
                diskInfo += "<br /><b><u>Клькість байтів у секторі:</u></b> ";
                diskInfo += QString::number(bytesPerSector);

                DWORD serialNumber;
                const DWORD fileSysNameSize = 7, volumeNameSize = 100;
                WCHAR fileSysName[fileSysNameSize] = {0};
                WCHAR volumeName[volumeNameSize] = {0};
                GetVolumeInformationW(diskName, volumeName, volumeNameSize, &serialNumber, nullptr, nullptr, fileSysName, fileSysNameSize);
                diskInfo += "<br /><b><u>Мітка диску:</u></b> ";
                diskInfo += QString::fromWCharArray(volumeName, volumeNameSize);
                diskInfo += "<br /><b><u>Серійний номер:</u></b> ";
                diskInfo += QString::number(serialNumber);
                diskInfo += "<br /><b><u>Файлова система:</u></b> ";
                diskInfo += QString::fromWCharArray(fileSysName, fileSysNameSize);
            }

            this->ui->textEdit->setText(diskInfo);
        });

    const DWORD size = GetLogicalDriveStringsW(0, nullptr);
    WCHAR *str = new WCHAR[size];
    GetLogicalDriveStringsW(size, str);
    QString diskList = QString::fromWCharArray(str, static_cast<int>(size) - 1);
    foreach (QString string, diskList.split(QString::fromWCharArray(&str[size - 1], 1)))
        if( !string.isEmpty() )
            ui->comboBox->addItem(string);

    delete[] str;
}

Widget::~Widget()
{
    delete ui;
}

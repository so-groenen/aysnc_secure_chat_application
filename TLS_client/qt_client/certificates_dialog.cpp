#include "certificates_dialog.h"
#include "ui_certificates_dialog.h"
#include <QDir>
#include <QFileInfo>

CertificatesDialog::CertificatesDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CertificatesDialog)
{
    ui->setupUi(this);
}

CertificatesDialog::~CertificatesDialog()
{
    delete ui;
}

void CertificatesDialog::on_PrivateButton_clicked()
{
    QString f = QFileDialog::getOpenFileName(this);
    qDebug() << f;
    QFileInfo file{f};
    ui->serverRootLabel->setText(file.fileName());

}


void CertificatesDialog::on_CertificateButton_clicked()
{
    QString f = QFileDialog::getOpenFileName(this);
    qDebug() << f;
    QFileInfo file{f};
    ui->serverRootLabel->setText(file.fileName());

}


void CertificatesDialog::on_ServerRootCAButton_clicked()
{
    QString f = QFileDialog::getOpenFileName(this);
    qDebug() << f;
    QFileInfo file{f};
    ui->serverRootLabel->setText(file.fileName() );

}


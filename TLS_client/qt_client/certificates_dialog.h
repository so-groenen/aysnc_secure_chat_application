#ifndef CERTIFICATES_DIALOG_H
#define CERTIFICATES_DIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QString>

namespace Ui {
class CertificatesDialog;
}

class CertificatesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CertificatesDialog(QWidget *parent = nullptr);
    ~CertificatesDialog();

private slots:
    void on_PrivateButton_clicked();

    void on_CertificateButton_clicked();

    void on_ServerRootCAButton_clicked();

private:
    Ui::CertificatesDialog *ui;
};

#endif // CERTIFICATES_DIALOG_H

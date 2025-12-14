#ifndef CERTIFICATES_DIALOG_H
#define CERTIFICATES_DIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QString>
#include <QDir>
#include <QFileInfo>
#include <QSslKey>
#include <QSslCertificate>
#include "security_bundle.h"
#include <QApplication>
#include <optional>
#include <filesystem>

namespace Ui {
class CertificatesDialog;
}

namespace fs = std::filesystem;

class CertificatesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CertificatesDialog(const fs::path& certs_dir, SecurityBundle&& keys_and_certs, QWidget *parent = nullptr);
    ~CertificatesDialog();
    SecurityBundle get_security_bundle() const;
private slots:
    void on_PrivateButton_clicked();
    void on_CertificateButton_clicked();
    void on_ServerRootCAButton_clicked();
private:
    void check_enable_ok();
private:
    Ui::CertificatesDialog *ui;
    bool m_has_private{};
    bool m_has_public{};
    bool m_has_rootCA{};
    SecurityBundle m_security_bundle{};
    std::optional<QDir> m_certs_dir{""};
};

#endif // CERTIFICATES_DIALOG_H

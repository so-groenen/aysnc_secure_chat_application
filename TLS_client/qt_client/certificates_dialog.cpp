#include "certificates_dialog.h"
#include "ui_certificates_dialog.h"

CertificatesDialog::CertificatesDialog(SecurityBundle &&keys_and_certs, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CertificatesDialog), m_security_bundle{std::move(keys_and_certs)}
{
    ui->setupUi(this);
    if(!m_certs_dir.has_value())
    {
        QDir dir = QDir(QApplication::applicationDirPath());//.root().root();
        if(dir.cdUp())
        {
            dir.cdUp();
            dir.cdUp();
            dir.cdUp();
            m_certs_dir = std::move(dir);
        }
    }
    if(!m_security_bundle.private_key_path.isEmpty())
    {
        ui->privateLabel->setText(QFileInfo(m_security_bundle.private_key_path).fileName());
        m_has_private = true;
    }
    if(!m_security_bundle.public_key_path.isEmpty())
    {
        ui->publicLabel->setText(QFileInfo(m_security_bundle.public_key_path).fileName());
        m_has_public = true;
    }
    if(!m_security_bundle.root_CA_path.isEmpty())
    {
        ui->serverRootLabel->setText(QFileInfo(m_security_bundle.root_CA_path).fileName());
        m_has_rootCA = true;
    }

    check_enable_ok();
}

CertificatesDialog::~CertificatesDialog()
{
    delete ui;
}

// QSslCertificate CertificatesDialog::get_root_CA() const
// {
//     return m_rootCA;
// }

// QSslKey CertificatesDialog::get_private_key() const
// {
//     return m_private_key;
// }

// QSslCertificate CertificatesDialog::get_public_key() const
// {
//     return m_local_certificate;
// }

SecurityBundle CertificatesDialog::get_security_bundle() const
{
    return m_security_bundle;
}

void CertificatesDialog::on_PrivateButton_clicked()
{
    m_has_private = false;

    QString key_path = QFileDialog::getOpenFileName(this, "Path to client private key [ex: client.key]", m_certs_dir.value().path());
    QFile key_file(key_path);
    QFileInfo key_file_info{key_file};

    if (!key_file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Could not open file " << key_file_info.fileName();
        return;
    }
    QSslKey private_key(key_file.readAll(), QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey);


    if(private_key.isNull())
    {
        QString error = "ERROR: " + key_file_info.fileName() + " is not valid!";
        ui->privateLabel->setText(error);
        return;
    }

    m_has_private = true;

    QString ok = "OK: " + key_file_info.fileName();
    ui->privateLabel->setText(ok);

    qDebug() << key_path;

    m_security_bundle.private_key      = std::move(private_key);
    m_security_bundle.private_key_path = std::move(key_path);
    check_enable_ok();
}


void CertificatesDialog::on_CertificateButton_clicked()
{
    m_has_public= false;
    QString public_key_path = QFileDialog::getOpenFileName(this, "Path to client public key [ex: client.crt]", m_certs_dir.value().path());;
    QFileInfo public_key_info{public_key_path};

    auto local_certificates = QSslCertificate::fromPath(public_key_path);


    if(local_certificates.isEmpty())
    {
        QString error = "ERROR: " + public_key_info.fileName() + " is not a valid file!";
        ui->publicLabel->setText(error);
        return;
    }
    if(auto local_cert = local_certificates.at(0); local_cert.isNull())
    {
        QString error = "ERROR: " + public_key_info.fileName() + " is not valid!";
        ui->publicLabel->setText(error);
        return;
    }

    auto local_cert = local_certificates.at(0);
    if(local_certificates.size() > 1)
    {
        QString error = "WARNING: " + public_key_info.fileName() + " contains more than one (first one taken)!";
        ui->publicLabel->setText(error);
    }
    else
    {
        QString ok = "OK: " + public_key_info.fileName();
        ui->publicLabel->setText(ok);
    }
    m_has_public = true;
    m_security_bundle.public_key      = std::move(local_cert);
    m_security_bundle.public_key_path = std::move(public_key_path);
    check_enable_ok();
}


void CertificatesDialog::on_ServerRootCAButton_clicked()
{
    m_has_rootCA = false;

    QString root_CA_path = QFileDialog::getOpenFileName(this, "Path to server root Certificate Authority  [ex: serverRootCA.crt]", m_certs_dir.value().path());;
    QFileInfo root_CA_info{root_CA_path};

    auto root_CAs = QSslCertificate::fromPath(root_CA_path);
    if(root_CAs.isEmpty())
    {
        QString error = "ERROR: " + root_CA_info.fileName() + " is not a valid file!";
        ui->serverRootLabel->setText(error);
        return;
    }
    if(auto root_CA = root_CAs.at(0); root_CA.isNull())
    {
        QString error = "ERROR: " + root_CA_info.fileName() + " is not valid!";
        ui->serverRootLabel->setText(error);
        return;
    }

    auto root_CA = root_CAs.at(0);
    if(root_CAs.size() > 1)
    {
        QString error = "WARNING: " + root_CA_info.fileName() + " contains more than one (first one taken)!";
        ui->serverRootLabel->setText(error);
    }
    else
    {
        QString ok = "OK: " + root_CA_info.fileName();
        ui->serverRootLabel->setText(ok);
    }

    m_has_rootCA = true;
    m_security_bundle.root_CA      = std::move(root_CA);
    m_security_bundle.root_CA_path = std::move(root_CA_path);
    check_enable_ok();
}

void CertificatesDialog::check_enable_ok()
{
    bool is_ok = (m_has_rootCA && m_has_public && m_has_private);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(is_ok);

}


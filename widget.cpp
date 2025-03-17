#include "widget.h"
#include "ui_widget.h"
#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    connect(ui->cbTcp, &QCheckBox::checkStateChanged, this, &Widget::on_cbTcp_checkStateChanged);
    connect(ui->cbSsl, &QCheckBox::checkStateChanged, this, &Widget::on_cbSsl_checkStateChanged);
}

Widget::~Widget()
{
    delete tcpSocket_;
    delete sslSocket_;
    delete ui;
}

void Widget::doConnected() {
    QString msg = "Connected to " + socket_->peerAddress().toString() + "\r\n";
    ui->pteMessage->insertPlainText(msg);
}

void Widget::doDisconnected() {
    QString msg = "Disconnected from " + socket_->peerAddress().toString() + "\r\n";
    ui->pteMessage->insertPlainText(msg);
}

void Widget::doReadyRead() {
    QByteArray ba = socket_->readAll();
    ui->pteMessage->insertPlainText(QString::fromUtf8(ba));
}



void Widget::on_pbConnect_clicked() {
    if (!ui->cbTcp->isChecked() && !ui->cbSsl->isChecked()) {
        QMessageBox::warning(this, "Warning", "check tcp or ssl");
        return;
    }
    if (useSsl_) {
        if (!sslSocket_) {
            sslSocket_ = new QSslSocket(this);
            connect(sslSocket_, &QSslSocket::connected, this, &Widget::doConnected);
            connect(sslSocket_, &QSslSocket::disconnected, this, &Widget::doDisconnected);
            connect(sslSocket_, &QSslSocket::readyRead, this, &Widget::doReadyRead);
        }
        socket_ = sslSocket_;
        sslSocket_->connectToHostEncrypted(ui->leHost->text(), ui->lePort->text().toUShort());
    } else {
        if (!tcpSocket_) {
            tcpSocket_ = new QTcpSocket(this);
            connect(tcpSocket_, &QTcpSocket::connected, this, &Widget::doConnected);
            connect(tcpSocket_, &QTcpSocket::disconnected, this, &Widget::doDisconnected);
            connect(tcpSocket_, &QTcpSocket::readyRead, this, &Widget::doReadyRead);
        }
        socket_ = tcpSocket_;
        tcpSocket_->connectToHost(ui->leHost->text(), ui->lePort->text().toUShort());
    }
}

void Widget::on_pbDisconnect_clicked() {
    if (socket_) {
        socket_->close();
    }
}

void Widget::on_pbClear_clicked() {
    ui->pteMessage->clear();
}


void Widget::on_pbSend_clicked() {
    if (socket_ && socket_->state() == QAbstractSocket::ConnectedState) {
        socket_->write(ui->pteSend->toPlainText().toUtf8());
    } else {
        ui->pteMessage->insertPlainText("Socket is not connected.\n");
    }
}

// void Widget::on_pteSend_textChanged() {

// }



void Widget::on_cbTcp_checkStateChanged(const Qt::CheckState &arg1)
{
    if (arg1 == Qt::Checked) {
        ui -> cbSsl -> setChecked(false);
        useSsl_ = false;
    }
}


void Widget::on_cbSsl_checkStateChanged(const Qt::CheckState &arg1)
{
    if (arg1 == Qt::Checked) {
        ui -> cbTcp -> setChecked(false);
        useSsl_ = true;
    }
}


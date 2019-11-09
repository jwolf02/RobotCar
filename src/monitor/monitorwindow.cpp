#include "monitorwindow.h"
#include "ui_monitorwindow.h"
#include<unistd.h>
#include <worker.h>

MonitorWindow::MonitorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MonitorWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MonitorWindow::update);
    timer->start(50);
    camera_enabled = true;
    worker::window = this;
}

MonitorWindow::~MonitorWindow()
{
    worker::disconnect();
    delete ui;
}

void MonitorWindow::update()
{
    // only update ui if something has actually changed
    if (modified.load(std::memory_order_consume)) {
        std::lock_guard<std::mutex> lock(this->mtx);
        ui->fps->setNum(this->fps);
        QString str;
        if (data_rate > 1000000000) {
            str = QString::number(data_rate / 1000000000) + " GB/s";
        } else if (data_rate > 1000000) {
            str = QString::number(data_rate / 1000000) + " MB/s";
        } else if (data_rate > 1000) {
            str = QString::number(data_rate / 1000) + " KB/s";
        } else {
            str = QString::number(data_rate) + " B/s";
        }
        ui->data_rate->setText(str);
        ui->width->setNum(this->width);
        ui->height->setNum(this->height);
        ui->message->setText(QString::fromStdString(msg));
        ui->image->setPixmap(this->pixmap);
        this->repaint();
        this->modified = false;
    }
}

void MonitorWindow::setFPS(int fps)
{
    std::lock_guard<std::mutex> lock(this->mtx);
    this->fps = fps;
    this->modified = true;
}

void MonitorWindow::setDataRate(unsigned int data_rate)
{
    std::lock_guard<std::mutex> lock(this->mtx);
    this->data_rate = data_rate;
    this->modified = true;
}

void MonitorWindow::setFrame(const cv::Mat &frame)
{
    std::lock_guard<std::mutex> lock(this->mtx);
    if (!frame.empty() && frame.isContinuous()) {
        pixmap = QPixmap::fromImage(QImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888));
    }
    this->modified = true;
}

void MonitorWindow::setMessage(const std::string &msg)
{
    std::lock_guard<std::mutex> lock(this->mtx);
    this->msg = msg;
    this->modified = true;
}

void MonitorWindow::setFrameSize(int width, int height)
{
    std::lock_guard<std::mutex> lock(this->mtx);
    this->width = width;
    this->height = height;
    this->modified = true;
}

bool MonitorWindow::cameraEnabled() const
{
    return camera_enabled;
}

void MonitorWindow::clear_ui()
{
    ui->status->setText("disconnected");
    ui->address->setEnabled(true);
    ui->port->setEnabled(true);
    ui->connection->setText("connect");
    setFPS(0);
    setDataRate(0);
    setFrameSize(0, 0);
    setFrame(cv::Mat());
    pixmap = QPixmap();
    ui->image->setPixmap(pixmap);
}

void MonitorWindow::on_connection_clicked()
{
    if (ui->connection->text() == "connect") {
        ui->connection->setEnabled(false);
        ui->address->setEnabled(false);
        ui->port->setEnabled(false);
        ui->connection->setText("connecting...");

        const std::string address = ui->address->text().toStdString();
        const int port = ui->port->text().toInt();

        if (!worker::connect(address, port)) {
            // failed connect
            ui->address->setEnabled(true);
            ui->port->setEnabled(true);
            ui->connection->setText("connect");
        } else {
            // successful connect
            ui->status->setText("connected");
            ui->connection->setText("disconnect");
            worker::run();
        }
        ui->connection->setEnabled(true);
    } else {
        clear_ui();
        worker::disconnect();
    }
}

void MonitorWindow::on_camera_clicked()
{
    worker::send_control('c');
    if (ui->camera->text() == "disable") {
        ui->camera->setText("enable");
        camera_enabled = false;
    } else {
        ui->camera->setText("disable");
        camera_enabled = true;
    }
}

void MonitorWindow::on_recording_clicked()
{
    if (ui->recording->text() == "start") {
        ui->recording->setText("stop");
    } else {
        ui->recording->setText("start");
    }
}

void MonitorWindow::on_MonitorWindow_destroyed()
{
    worker::send_control('x');
    worker::disconnect();
    clear_ui();
}

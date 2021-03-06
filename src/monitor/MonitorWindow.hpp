#ifndef __MONITORWINDOW_HPP
#define __MONITORWINDOW_HPP

#include <QMainWindow>
#include <QTimer>
#include <QEvent>
#include <mutex>
#include <atomic>
#include <opencv2/opencv.hpp>

namespace Ui {
    class MonitorWindow;
}

class MonitorWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MonitorWindow(QWidget *parent = nullptr);

    ~MonitorWindow() override;

    void setFPS(int fps);

    void setFrame(const cv::Mat &frame);

    void setDataRate(unsigned int data_rate);

    void setMessage(const std::string &msg);

    void setFrameSize(int width, int height);

    void setPing(int ping);

protected:
    bool eventFilter(QObject *o, QEvent *e) override;

private slots:
    void update_ui();

private slots:
    void on_connection_clicked();

    void on_recording_clicked();

    void on_MonitorWindow_destroyed();

private:
    void clear_ui();

    void disconnect();

    Ui::MonitorWindow *ui = nullptr;

    QTimer *timer = nullptr;

    std::mutex mtx;

    std::atomic_bool modified;

    QPixmap pixmap;

    int fps = 0;

    unsigned int data_rate = 0; // in B/s

    int width = 0;

    int height = 0;

    int ping = 0;

    std::string msg;

};

#endif // __MONITORWINDOW_HPP

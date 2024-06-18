#pragma once
#pragma execution_character_set("utf-8")

#include <QThread>
#include <functional>

class WorkerThread : public QThread {
    Q_OBJECT

public:
    WorkerThread(std::function<void()> task, QObject* parent = nullptr)
        : QThread(parent), m_task(task) {}

    void run() override {
        if (m_task) {
            m_task();
        }
    }

private:
    std::function<void()> m_task;
};

#ifndef PROGRESSBUTTONWIDGET_H
#define PROGRESSBUTTONWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QPaintEvent>
#include <QDebug>
#include <QVariantAnimation>
class progressButtonWidget : public QWidget
{
    Q_OBJECT

public:
    progressButtonWidget(QWidget *parent = nullptr);
    ~progressButtonWidget();

signals:
    void startProcessing();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    enum class state
    {
        normal,
        hover,
        fromRoundedCornerToRounded, // 圆角变圆形
        openProgress,
        closeProgress,
        recovery
    };
    state buttonState{state::normal};
    QString text{"我是按钮"};
    QTimer timer;
    int widthChangeValue{0};
    int progress{0}; // 百分比

private:
    void onTimeout();
    void operationProcessing();

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
};

class WaterDrop : public QWidget
{
  Q_OBJECT

public:
    WaterDrop(QWidget *parent = nullptr);
    void show();
    void move(const QPoint &point);
private:
    void paintEvent(QPaintEvent *event) override;
    void onRaduisChanged(QVariant value);

private:
    class QVariantAnimation *m_waterDropAnimation;
    int m_animationRaduis;
};


#endif // PROGRESSBUTTONWIDGET_H

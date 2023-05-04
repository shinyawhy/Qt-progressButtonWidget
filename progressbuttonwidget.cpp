#include "progressbuttonwidget.h"


progressButtonWidget::progressButtonWidget(QWidget *parent)
    : QWidget(parent)
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    setFixedHeight(60);
    setMinimumWidth(140);
    auto font = this->font();
    font.setPixelSize(24);
    setFont(font);

    setMouseTracking(true);

    connect(&timer, &QTimer::timeout, this, &progressButtonWidget::onTimeout);
    connect(this, &progressButtonWidget::startProcessing, this, &progressButtonWidget::operationProcessing, Qt::QueuedConnection); // 主要是让水滴动画延迟一会出现 避免阻塞
    timer.setInterval(40);
}

progressButtonWidget::~progressButtonWidget()
{
}

void progressButtonWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    auto rect = event->rect();

    if(buttonState == state::normal || buttonState == state::hover)
    {
        QPainterPath path;
        path.addRoundedRect(rect, 30, 30);
        painter.setClipPath(path);
        painter.fillRect(rect, Qt::white);

        painter.save();
        painter.setBrush(QColor(buttonState == state::normal ? "#68945c" : "#446a37"));
        painter.setPen(Qt::transparent);
        painter.drawRoundedRect(rect.adjusted(3,3,-3,-3), 30, 30);
        painter.restore();

        painter.setPen(Qt::white);
        painter.setFont(this->font());
        painter.drawText(rect, Qt::AlignCenter, text);

    }
    else if(buttonState == state::fromRoundedCornerToRounded || buttonState == state::recovery)
    {
        painter.setBrush(QColor("#68945c"));
        painter.setPen(QPen(QColor("#84a729")));
        painter.translate(rect.center());
        painter.drawRoundedRect(QRect(-widthChangeValue, -(rect.height() / 2 - 3), widthChangeValue * 2, rect.height() - 6), 30, 30);
    }
    else if(buttonState == state::openProgress)
    {
        painter.translate(rect.center());
        auto radiu = (rect.height() - 6) / 2 - 3;
        painter.setBrush(QColor("#68945c"));
        painter.setPen(QPen(QColor("#84a729")));
        painter.drawEllipse(QPoint(0, 0), radiu, radiu);

        painter.setPen(QPen(Qt::white, 3));

        QRect rect = QRect(-radiu, -radiu,
                           radiu*2, radiu*2);

        auto angle = progress * 360 / 100;
        painter.drawArc(rect.adjusted(-3, -3, 3, 3), 90 * 16, -static_cast<int>(angle *16));
    }
    else if(buttonState == state::closeProgress)
    {
        auto radiu = (rect.height() - 6) / 2;
        painter.translate(rect.center());
        painter.setPen(Qt::transparent);
        painter.setBrush(QColor(0, 0, 0, 63));
        painter.drawEllipse(QPoint(0,0), radiu, radiu);

        radiu -= 3;
        painter.setBrush(Qt::white);
        painter.drawEllipse(QPoint(0, 0), radiu, radiu);

        painter.setPen(QPen(QColor("#446a37"),3,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        painter.drawLine(QPoint(-radiu / 3,0),
                         QPoint(-radiu / 5,radiu / 3));
        painter.drawLine(QPoint(-radiu / 5,radiu / 3),
                         QPoint(radiu / 4,-radiu / 4));

    }
}

void progressButtonWidget::onTimeout()
{
    if(buttonState == state::fromRoundedCornerToRounded)
    {
        widthChangeValue -= (this->width() *0.05);
        if(widthChangeValue <= (this->height()))
        {
            buttonState = state::openProgress;
            progress = 0;
            timer.stop();
            emit startProcessing();
        }
    }
    else
    {
        widthChangeValue +=(this->width()* 0.05);
        if(widthChangeValue >= (this->width() / 2))
        {
            buttonState = state::normal;
            timer.stop();
        }
    }

    update();

}

void progressButtonWidget::operationProcessing()
{
    // 进度条的时间 主要耗时体现在这
    for(int i = 0; i < 500000000; i++)
    {
        if(i % 5000000 == 0)
        {
            ++progress;
            repaint();
        }
    }

    if(progress == 100)
    {
        buttonState = state::closeProgress;
        update();
        auto waterDrop = new WaterDrop();
        waterDrop->move(this->mapToGlobal(this->rect().center()));
        waterDrop->show();
    }

}




void progressButtonWidget::enterEvent(QEvent *event)
{
    if(buttonState == state::normal)
    {
        buttonState = state::hover;
        update();
    }
    QWidget::enterEvent(event);
}

void progressButtonWidget::leaveEvent(QEvent *event)
{
    if(buttonState == state::hover)
    {
        buttonState = state::normal;
        update();
    }
    QWidget::leaveEvent(event);
}

void progressButtonWidget::mousePressEvent(QMouseEvent *event)
{
    if(buttonState == state::hover || buttonState == state::normal)
    {
        buttonState = state::fromRoundedCornerToRounded;
        widthChangeValue = (this->width() - 6) / 2;
        timer.start();
        update();
    }
    else if(buttonState == state::closeProgress)
    {

        buttonState == state::recovery;
        timer.start();
        update();
    }
    QWidget::mousePressEvent(event);
}

const int RADUIS = 60;
WaterDrop::WaterDrop(QWidget *parent)
    :QWidget(parent),
      m_waterDropAnimation(nullptr),
      m_animationRaduis(0)
{
    this->setFixedSize(QSize(RADUIS * 2, RADUIS * 2));
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setAttribute(Qt::WA_DeleteOnClose);

    m_waterDropAnimation = new QVariantAnimation(this);
}

void WaterDrop::show()
{
    m_waterDropAnimation->setStartValue(0);
    m_waterDropAnimation->setEndValue(RADUIS);
    m_waterDropAnimation->setDuration(350);

    connect(m_waterDropAnimation, &QVariantAnimation::valueChanged, this, &WaterDrop::onRaduisChanged);
    connect(m_waterDropAnimation, &QVariantAnimation::finished, this, &WaterDrop::close);

    m_waterDropAnimation->start();
    QWidget::show();
}
// 点击点转换为圆心点的坐标
void WaterDrop::move(const QPoint &point)
{
    QPoint translatePoint = point - QPoint(RADUIS, RADUIS);
    QWidget::move(translatePoint);
}

void WaterDrop::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen;
    pen.setColor(QColor(0xffffff80));
    pen.setWidth(5);
    painter.setPen(pen);
    painter.drawEllipse(event->rect().center(), m_animationRaduis, m_animationRaduis);
}

void WaterDrop::onRaduisChanged(QVariant value)
{
    m_animationRaduis = value.toInt();
    update();
}

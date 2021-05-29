#include "view.h"

View::View(const QSize& size, QWidget* parent) : QLabel{parent}, initPixmap{size}
{   /* prepare view size */
    initPixmap.fill(QColor(Qt::white));
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    setPixmap(initPixmap);
    origPixmap = initPixmap;    // Copy in order to keep it

    setOrigRatio();             // Calculate scale coefficients
}

void View::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        inDrawMode = true;  // Begin to draw

        x = event->x();
        y = event->y();

        initPixmap = *pixmap();   // Copy what we draw

        if(dMode == Poly)
        {
             /* Tracking mode for drawing polygon */
             setMouseTracking(true);
             points << QPoint(x, y);
         }
    }
    else if((event->buttons() & Qt::RightButton) && !points.isEmpty())
    {    /* If right button was clicked, we save polygon */
         points << points.first();
         setMouseTracking(false);
         curX = x; curY = y;   // Treating last line in order to it isn't pulled

         QPainter painter;
         QPixmap tPixmap = initPixmap;
         painter.begin(&tPixmap);
         painter.setPen(pen);
         painter.setBrush(brush);
         painter.drawPolygon(points);
         painter.end();
         setPixmap(tPixmap);

         /* Transform coordinates for original image */
         for(QPoint& item: points)
         {
            item.rx() = qRound(static_cast<float>(item.x()) * xRatio);
            item.ry() = qRound(static_cast<float>(item.y()) * yRatio);
         }

         /* Draw on original image */
         QPen tPen{ pen };
         int tPenWidth{ qRound(static_cast<float>(pen.width()) * xRatio) };   // Scaling pen width
         tPen.setWidth(tPenWidth);
         painter.begin(&origPixmap);
         painter.setPen(tPen);
         painter.setBrush(brush);
         painter.drawPolygon(points);
         painter.end();
         points.clear();

         inDrawMode = false;
         fileChanged = true;
         emit fileChange();
     }
}

void View::paintEvent(QPaintEvent *event)
{
    /* Draw only when button is pressed */
    if(inDrawMode)
    {
        QPainter painter;
        QPixmap tPixmap = initPixmap;    // Draw on current image
        painter.begin(&tPixmap);
        painter.setPen(pen);
        painter.setBrush(brush);
        switch(dMode)
        {
        case Line:
            painter.drawLine(x, y, curX, curY);
            break;
        case Rect:
            painter.drawRect(x, y, curX - x, curY - y);
            break;
        case Ellipse:
            painter.drawEllipse(x, y, curX - x, curY - y);
            break;
        case JustPen:
            painter.drawPoint(curX, curY);
            points << QPoint(curX, curY);   // Save to draw on original image
            break;
        case Poly:
            painter.drawPolyline(points);
            painter.drawLine(x, y, curX, curY);
            break;
        }
        painter.end();
        setPixmap(tPixmap); // Show result in widget

        if(dMode == JustPen)
            initPixmap = std::move(tPixmap);
    }
    QLabel::paintEvent(event);
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton && dMode != JustPen)
    {   
        curX = event->x();
        curY = event->y();

        update();
    }
    else
    {
        curX = event->x();
        curY = event->y();
        update();
    }
}

void View::mouseReleaseEvent(QMouseEvent *event)
{
    /* Draw on original image given the scale */
    QPainter painter;
    QPixmap tPixmap = origPixmap;
    QPen tPen{ pen };
    int tPenWidth{ qRound(static_cast<float>(pen.width()) * xRatio) };
    int tX{ qRound(static_cast<float>(x) * xRatio) },
        tY{ qRound(static_cast<float>(y) * yRatio) },
        tCurX{ qRound(static_cast<float>(curX) * xRatio) },
        tCurY{ qRound(static_cast<float>(curY) * yRatio) };
    tPen.setWidth(tPenWidth);
    painter.begin(&tPixmap);
    painter.setPen(tPen);
    painter.setBrush(brush);

    switch(dMode)
    {
    case Line:
        painter.drawLine(tX, tY, tCurX, tCurY);
        inDrawMode = false; // Button was released, stop drawing
        break;
    case Rect:
        painter.drawRect(tX, tY, tCurX - tX, tCurY - tY);
        inDrawMode = false;
        break;
    case Ellipse:
        painter.drawEllipse(tX, tY, tCurX - tX, tCurY - tY);
        inDrawMode = false;
        break;
    case JustPen:
        for(QPoint& item: points)
        {
           item.rx() = qRound(static_cast<float>(item.x()) * xRatio);
           item.ry() = qRound(static_cast<float>(item.y()) * yRatio);
        }

        painter.drawPoints(points);
        points.clear();
        inDrawMode = false;
        break;
    }
    painter.end();
    fileChanged = true;
    emit fileChange();
    origPixmap = tPixmap;   // Save what we drew

    QLabel::mouseReleaseEvent(event);
}

QPen &View::getPen() noexcept
{
    return pen;
}

QBrush &View::getBrush() noexcept
{
    return brush;
}

void View::setDrawMode(View::DrawMode mode) noexcept
{
    dMode = mode;
}

void View::setTypeColor(View::TypeColor tColor) noexcept
{
    whoseColor = tColor;
}

View::TypeColor View::getTypeColor() const
{
    return whoseColor;
}

QPixmap &View::getPixmap() noexcept
{
    return initPixmap;
}

QPixmap &View::getOrigPixmap() noexcept
{
    return origPixmap;
}

bool View::setOrigRatio() noexcept
{
    if(origPixmap.isNull() || initPixmap.isNull()) // If there is empty image, can't init scale coefficients
        return false;

    xRatio = static_cast<float>(origPixmap.width()) / initPixmap.width();
    yRatio = static_cast<float>(origPixmap.height()) / initPixmap.height();

    return true;
}

bool View::isFileChange() const noexcept
{
    return fileChanged;
}

void View::setChangedFile(bool flag) noexcept
{
    fileChanged = flag;
}


#ifndef VIEW_H
#define VIEW_H

#include <QLabel>
#include <QMouseEvent>
#include <QVector>
#include <QPainterPath>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QPixmap>
#include <QSizePolicy>

class View: public QLabel
{
    Q_OBJECT

public:
    enum DrawMode{Line, Rect, Ellipse, Poly, JustPen};
    enum TypeColor{Pen, Brush};

    View(const QSize& size, QWidget* parent = nullptr);
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    QPen& getPen() noexcept;
    QBrush& getBrush() noexcept;
    void setDrawMode(DrawMode mode) noexcept;
    void setTypeColor(TypeColor tColor) noexcept;
    TypeColor getTypeColor() const;
    QPixmap& getPixmap() noexcept;
    QPixmap& getOrigPixmap() noexcept;
    bool setOrigRatio() noexcept;
    bool isFileChange() const noexcept;
    void setChangedFile(bool = false) noexcept;

signals:
    void fileChange();

private:
    int x, y, curX, curY;
    float xRatio{0.0f}, yRatio{0.0f};

    /* Original and downloaded images. We segregate them in order not to spoil original image resolution. */
    QPixmap initPixmap{1, 1};
    QPixmap origPixmap;
    QPen pen;
    QBrush brush;
    QVector<QPoint> points;
    DrawMode dMode{Line};
    TypeColor whoseColor;
    bool inDrawMode{false};
    bool fileChanged{false};

};

#endif // VIEW_H

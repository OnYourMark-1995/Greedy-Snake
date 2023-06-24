
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QList>
#include<QRectF>
#include<QPaintEvent>
#include<QPainter>
#include<QBrush>
#include<QRgb>
#include<QColor>
#include<QTimerEvent>
#include<QTimer>
#include<QPushButton>
#include<QDebug>
#include<QMessageBox>
#include<QString>
#include<QRandomGenerator>
#include<QPixmap>
#include<QScreen>
#include<QPoint>


enum Direction{UP ,DOWN ,LEFT ,RIGHT};
enum GameState{WAIT, START, PAUSE, OVER};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void initializeGame();
    void move_up();
    void move_down();
    void move_left();
    void move_right();
    void snake_move();
    void createFood();
    bool isEatFood();
    bool isGameOver();

protected:
    void paintEvent(QPaintEvent *ev);
    void keyPressEvent(QKeyEvent * event);

signals:
    void gameOver();

private:

    Ui::MainWindow *ui;

    QList<QRectF> snake;//存储蛇的每一节信息。QRectF中保存(x, y, width, height)，用于记录蛇每一节在局部坐标系中的位置以及宽高
    int snakeSize=20;//蛇每一节的边长，决定蛇的大小
    int snakeStartLength=9;//游戏开始时蛇的长度
    QRgb snake_headColor=qRgb(0,235,0);//蛇头颜色
    QRgb snake_bodyColor=qRgb(180,180,180);//蛇身颜色
    Direction snakeMoveDirection=RIGHT;//蛇移动的方向，游戏开始时为向右
    int startPointX=snakeSize*17;//游戏开始时蛇头的位置
    int startPointY=snakeSize*12;

    QList<QRectF> foods;//食物用Qlist存储，在屏幕中一次显示3个
    QRgb food_color=qRgb(255,140,0);//食物颜色

    QRgb brush_color=qRgb(220,220,220);

    QTimer *paintTimer;
    int paintTimeInterval=125;//paintEvent的定时器的时间间隔，决定蛇的移动速度

    int score=0;//游戏分数，即吃到的食物个数
    int level_score[6]={1,3,5,7,9,11};//到达一定分数增长一节
    int level=1;
    bool isGrow=false;//标记下一次更新是否增加一节
    GameState gameState=WAIT;//游戏状态：等待，开始，暂停，结束

    QString background_img=":/new/prefix1/background/background8.png";
};

#endif // MAINWINDOW_H

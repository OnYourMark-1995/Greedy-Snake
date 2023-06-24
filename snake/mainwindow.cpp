
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //根据显示器的不同分辨率设置窗口大小
    QScreen *screen = qApp->primaryScreen();
    qreal dpiVal = screen->logicalDotsPerInch();

    if(dpiVal==96){
        snakeSize=25;
    }
    else if(dpiVal==120){
        snakeSize=31;
    }
    else if(dpiVal==144){
        snakeSize=37;
    }
    else if(dpiVal==168){
        snakeSize=44;
    }
    else if(dpiVal==192){
        snakeSize=50;
    }

    startPointX=snakeSize*17;//游戏开始时蛇头的位置
    startPointY=snakeSize*12;

    //设置窗口标题和大小
    this->setWindowTitle("greedy snake");
    this->setFixedSize(snakeSize*32,snakeSize*22);//设定窗口固定大小

    //初始化游戏
    initializeGame();
    createFood();

    //开始游戏按钮设置
    QPushButton *startBtn=new QPushButton("开始游戏",this);
    startBtn->show();
    startBtn->resize(snakeSize*6,snakeSize*3);
    startBtn->move(this->width()/2-snakeSize*3,this->height()/2-snakeSize*2);
    startBtn->setStyleSheet(
        //正常状态样式
        "QPushButton{"
        "background-color:#67a6f8;"//设置按钮背景色
        "border-radius:5px;"//设置圆角半径
        "color:white;"//设置按钮字体颜色
        "font-size:40px"
        "}"

        //鼠标悬停样式
        "QPushButton:hover{"
        "background-color:#5f9ae6;"
        "color:white;"
        "}");

    setFocusPolicy(Qt::StrongFocus);//设置窗口焦点，否则开始按钮会影响空格键暂停功能

    connect(startBtn,&QPushButton::clicked,this,[=](){
        gameState=START;
        startBtn->hide();
        paintTimer->start();
    });

    // 游戏结束弹窗
    connect(this,&MainWindow::gameOver,this,[=](){
        QString str1=QString("游戏结束!   最终得分:%1   \n\n    要再来一局吗").arg(score);
        if(QMessageBox::Yes==QMessageBox::question(this,"Game Over",str1)){
            initializeGame();
            gameState=START;
        }
        else{
            this->close();
        }
    });

    //菜单栏部分
    QMenu *menuInstructions=new QMenu("说明");
    QAction *actionGuide=new QAction("操作说明");
    QAction *actionAbout=new QAction("关于");
    QList<QAction*> list;
    list.append(actionGuide);
    list.append(actionAbout);
    menuInstructions->addActions(list);
    QMenuBar *menuBar=new QMenuBar;
    menuBar->addMenu(menuInstructions);
    this->setMenuBar(menuBar);

    connect(actionGuide,&QAction::triggered,this,[=](){
        QMessageBox::information(this,"操作说明","通过'w'，'a，,'s'，'d'控制贪吃蛇上下左右移动。\n按下空格键可暂停，再按一次空格键可继续游戏。");
    });

    connect(actionAbout,&QAction::triggered,this,[=](){
        QMessageBox::information(this,"关于","一款使用qt开发的贪吃蛇小游戏，祝你玩得开心 ^_^");
    });

    //游戏主体部分，通过计时器更新，不断绘制贪吃蛇
    paintTimer=new QTimer();
    paintTimer->setInterval(paintTimeInterval);
    connect(paintTimer,&QTimer::timeout,this,[=](){
        if(gameState==START) snake_move();

        if(isEatFood()){
            createFood();
            if(score==level_score[level]){
                isGrow=true;
                level++;
            }
        }

        if(isGameOver()){
            gameState=OVER;
            emit gameOver();
        }
        this->update();
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *ev){
    QPainter painter(this);
    QPixmap pix;
    pix.load(background_img);
    painter.drawPixmap(0,0,this->width(),this->height(),pix);

    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(5);
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);

    QColor color;

    if(gameState==START||gameState==PAUSE||gameState==OVER){
        color.setRgb(brush_color);
        brush.setColor(color);
        painter.setBrush(brush);

        color.setRgb(food_color);
        pen.setColor(color);
        painter.setPen(pen);

        for(int i=0;i<foods.length();i++){//绘制食物
            painter.drawRect(foods[i]);
        }

        color.setRgb(snake_bodyColor);
        pen.setColor(color);
        painter.setPen(pen);
        for(int i=1;i<snake.length();i++){//绘制蛇身
            painter.drawRect(snake[i]);
        }

        color.setRgb(snake_headColor);
        pen.setColor(color);
        painter.setPen(pen);
        painter.drawRect(snake[0]);//绘制蛇头（颜色与蛇身不同）
    }

    if(gameState==PAUSE){
        painter.setPen(QColor(255,255,255));
        painter.setFont(QFont("黑体",20));
        QString str10=QString("游戏暂停!");
        QString str11=QString("请按空格键继续!");

        painter.drawText(QPoint(this->width()/2-85,this->height()/2-40),str10);
        painter.drawText(QPoint(this->width()/2-145,this->height()/2+40),str11);
    }
}

void MainWindow::initializeGame(){
    snake.clear();
    snakeMoveDirection=RIGHT;
    for(int i=0;i<snakeStartLength;i++){//初始化蛇
        snake.push_back(QRectF(startPointX-snakeSize*i,startPointY,snakeSize,snakeSize));
    }
    score=0;
    level=1;
//    gameState=START;
}

void MainWindow::createFood(){
    int x,y;
    while(foods.size()<2){
        x=QRandomGenerator::global()->bounded(0, 32);
        y=QRandomGenerator::global()->bounded(1, 22);//高度从1开始，生成在0行会被菜单栏挡住
        foods.push_back(QRectF(x*snakeSize,y*snakeSize,snakeSize,snakeSize));
    }
}

void MainWindow::move_up(){
    snake.push_front(QRectF(snake.front().x(),snake.front().y()-snakeSize,snakeSize,snakeSize));
    if(!isGrow) snake.pop_back();
    else isGrow=false;
}

void MainWindow::move_down(){
    snake.push_front(QRectF(snake.front().x(),snake.front().y()+snakeSize,snakeSize,snakeSize));
    if(!isGrow) snake.pop_back();
    else isGrow=false;
}

void MainWindow::move_left(){
    snake.push_front(QRectF(snake.front().x()-snakeSize,snake.front().y(),snakeSize,snakeSize));
    if(!isGrow) snake.pop_back();
    else isGrow=false;
}

void MainWindow::move_right(){
    snake.push_front(QRectF(snake.front().x()+snakeSize,snake.front().y(),snakeSize,snakeSize));
    if(!isGrow) snake.pop_back();
    else isGrow=false;
}

void MainWindow::keyPressEvent(QKeyEvent * event){
    switch (event->key())
    {
    case Qt::Key_W:
        if(snakeMoveDirection!=DOWN){
            snakeMoveDirection=UP;
        }
        break;
    case Qt::Key_S:
        if(snakeMoveDirection!=UP){
            snakeMoveDirection=DOWN;
        }
        break;
    case Qt::Key_A:
        if(snakeMoveDirection!=RIGHT){
            snakeMoveDirection=LEFT;
        }
        break;
    case Qt::Key_D:
        if(snakeMoveDirection!=LEFT){
            snakeMoveDirection=RIGHT;
        }
        break;
    case Qt::Key_Space:
        if(gameState==START){
            gameState=PAUSE;
            paintTimer->stop();
            this->update();
        }
        else if(gameState==PAUSE){
            gameState=START;
            paintTimer->start();
        }
        break;
    }
}

void MainWindow::snake_move(){
    switch(snakeMoveDirection){
    case UP:
        move_up();
        break;
    case DOWN:
        move_down();
        break;
    case LEFT:
        move_left();
        break;
    case RIGHT:
        move_right();
        break;
    }
}

bool MainWindow::isEatFood(){
    for(int i=0;i<foods.length();i++){
        if(snake[0].x()==foods[i].x()&&snake[0].y()==foods[i].y()){
            score++;
            foods.removeAt(i);
            return true;
        }
    }
    return false;
}

bool MainWindow::isGameOver(){
    for(int i=1;i<snake.length();i++){
        if(snake.front().x()==snake[i].x()&&snake.front().y()==snake[i].y()){
//            qDebug()<<"eat self";
            return true;
        }
    }
    switch(snakeMoveDirection){
    case UP:
        if(snake.front().y()==0) return true;
        break;
    case DOWN:
        if(snake.front().y()==this->height()) return true;
        break;
    case LEFT:
        if(snake.front().x()==-snakeSize) return true;
        break;
    case RIGHT:
        if(snake.front().x()==this->width()) return true;
        break;
    }

    return false;
}

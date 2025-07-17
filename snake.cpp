#include "snake.h"
#include <QPainter>
#include <QTime>
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QDebug>

Snake::Snake(QWidget *parent) : QWidget(parent), dir(Right), inGame(Menu), score(0), highScore(0), timeRemaining(TIMEOUT_SECONDS) {
    setFixedSize(BOARD_WIDTH * 20, BOARD_HEIGHT * 20);

    QVBoxLayout *layout = new QVBoxLayout(this);
    playButton = new QPushButton("Play", this);
    exitButton = new QPushButton("Exit", this);
    highScoreButton = new QPushButton("High Score", this);

    layout->addWidget(playButton);
    layout->addWidget(highScoreButton);
    layout->addWidget(exitButton);

    connect(playButton, &QPushButton::clicked, this, &Snake::onPlayClicked);
    connect(exitButton, &QPushButton::clicked, this, &Snake::onExitClicked);
    connect(highScoreButton, &QPushButton::clicked, this, &Snake::onHighScoreClicked);

    setLayout(layout);

    loadHighScore();
}

Snake::~Snake() {
    delete gameTimer;
    delete moveTimer;
}

void Snake::startGame() {
    snake.clear();
    snake.append({BOARD_WIDTH / 2, BOARD_HEIGHT / 2});
    for (int i = 1; i < INIT_LENGTH; i++) {
        snake.append({BOARD_WIDTH / 2 - i, BOARD_HEIGHT / 2});
    }
    placeFood();

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &Snake::onTimeout);
    gameTimer->start(1000);

    moveTimer = new QTimer(this);
    connect(moveTimer, &QTimer::timeout, this, &Snake::onMoveTimeout);
    moveTimer->start(MOVE_INTERVAL);

    inGame = Game;
    score = 0;
    timeRemaining = TIMEOUT_SECONDS;

    playButton->hide();
    exitButton->hide();
    highScoreButton->hide();

    setFocusPolicy(Qt::StrongFocus);
    setFocus();
}

void Snake::paintEvent(QPaintEvent *event) {
    QPainter qp(this);
    drawBoard(qp);
}

void Snake::keyPressEvent(QKeyEvent *event) {
    if (inGame == Game) {
        switch (event->key()) {
            case Qt::Key_Left:
                if (dir != Right) dir = Left;
                break;
            case Qt::Key_Right:
                if (dir != Left) dir = Right;
                break;
            case Qt::Key_Up:
                if (dir != Down) dir = Up;
                break;
            case Qt::Key_Down:
                if (dir != Up) dir = Down;
                break;
            default:
                QWidget::keyPressEvent(event);
        }
    } else if (event->key() == Qt::Key_Escape && (inGame == Over || inGame == ShowHighScore)) {
        reset();
        repaint();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void Snake::onTimeout() {
    if (inGame == Game) {
        timeRemaining--;
        if (timeRemaining <= 0) {
            inGame = Over;
            gameOver();
        }
        repaint();
    }
}

void Snake::onMoveTimeout() {
    if (inGame == Game) {
        move();
        checkCollision();
        repaint();
    }
}

void Snake::move() {
    for (int i = snake.size() - 1; i > 0; --i) {
        snake[i] = snake[i - 1];
    }

    switch (dir) {
        case Left:
            snake[0].x--;
            break;
        case Right:
            snake[0].x++;
            break;
        case Up:
            snake[0].y--;
            break;
        case Down:
            snake[0].y++;
            break;
    }
}

void Snake::checkCollision() {
    if (snake[0].x < 0 || snake[0].x >= BOARD_WIDTH || snake[0].y < 0 || snake[0].y >= BOARD_HEIGHT) {
        inGame = Over;
        gameOver();
    }

    for (int i = 1; i < snake.size(); ++i) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            inGame = Over;
            gameOver();
        }
    }

    if (snake[0].x == food.x && snake[0].y == food.y) {
        snake.append({-1, -1});
        score++;
        timeRemaining = TIMEOUT_SECONDS;
        placeFood();
    }
}

void Snake::placeFood() {
    food.x = qrand() % BOARD_WIDTH;
    food.y = qrand() % BOARD_HEIGHT;
}

void Snake::drawBoard(QPainter &qp) {
    qp.fillRect(rect(), QColor(20, 20, 20));
    if (inGame == Game) {
        qp.setBrush(Qt::red);
        qp.drawRect(food.x * UNIT_SIZE, food.y * UNIT_SIZE, UNIT_SIZE, UNIT_SIZE);

        for (int i = 0; i < snake.size(); ++i) {
            if (i == 0)
                qp.setBrush(Qt::green);
            else
                qp.setBrush(Qt::darkGreen);
            qp.drawRect(snake[i].x * UNIT_SIZE, snake[i].y * UNIT_SIZE, UNIT_SIZE, UNIT_SIZE);
        }

        QString timeText = QString("Time Remaining: %1").arg(timeRemaining);
        if (timeRemaining <= 1) {
            qp.setPen(Qt::red);
        } else if (timeRemaining <= 2) {
            qp.setPen(Qt::yellow);
        } else {
            qp.setPen(Qt::white);
        }
        qp.drawText(rect().topRight() - QPoint(150, -20), timeText);
    } else if (inGame == Over) {
        drawGameOver(qp);
    } else if (inGame == Menu) {
        drawMenu(qp);
    } else if (inGame == ShowHighScore) {
        drawHighScore(qp);
    }
}

void Snake::drawMenu(QPainter &qp) {
    qp.fillRect(rect(), QColor(20, 20, 20));
    QBrush brush(Qt::darkCyan, Qt::FDiagPattern);
    qp.setBrush(brush);
    qp.drawRect(rect());
    playButton->show();
    exitButton->show();
    highScoreButton->show();

    QFont font("Courier", 40, QFont::Bold);
    QFontMetrics fm(font);
    int textWidth = fm.width("SNAKE");
    int textHeight = fm.height();

    qp.setFont(font);
    qp.setPen(Qt::green);
    qp.drawText(width() / 2 - textWidth / 2, textHeight / 2 + 5, "SNAKE");
}

void Snake::drawGameOver(QPainter &qp) {
    qp.fillRect(rect(), QColor(20, 20, 20));
    QStringList lines;
    lines << "Game Over"
          << ""
          << "Score: " + QString::number(score)
          << "High Score: " + QString::number(highScore)
          << ""
          << "Press ESC to return to menu";

    QFont font("Courier", 15, QFont::DemiBold);
    qp.setFont(font);
    qp.setPen(Qt::white);
    qp.drawText(rect(), Qt::AlignCenter, lines.join("\n"));
}

void Snake::drawHighScore(QPainter &qp) {
    QStringList lines;
    lines << "High Score: " + QString::number(highScore)
          << ""
          << "Press ESC to return to menu";

    QFont font("Courier", 15, QFont::DemiBold);
    qp.setFont(font);
    qp.drawText(rect(), Qt::AlignCenter, lines.join("\n"));
}

void Snake::gameOver() {
    gameTimer->stop();
    moveTimer->stop();
    if (score > highScore) {
        highScore = score;
        saveHighScore();
    }
    repaint();
}

void Snake::reset() {
    inGame = Menu;
    snake.clear();
    score = 0;
    playButton->show();
    exitButton->show();
    highScoreButton->show();
    repaint();
}

void Snake::loadHighScore() {
    QFile file("highscore.txt");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        in >> highScore;
        file.close();
    }
}

void Snake::saveHighScore() {
    QFile file("highscore.txt");
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        out << highScore;
        file.close();
    }
}

void Snake::onPlayClicked() {
    startGame();
}

void Snake::onExitClicked() {
    close();
}

void Snake::onHighScoreClicked() {
    inGame = ShowHighScore;
    playButton->hide();
    exitButton->hide();
    highScoreButton->hide();
    repaint();
}

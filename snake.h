#ifndef SNAKEGAME_H
#define SNAKEGAME_H

#include <QWidget>
#include <QKeyEvent>
#include <QTimer>
#include <QPushButton>

class Snake : public QWidget {
    Q_OBJECT

public:
    Snake(QWidget *parent = nullptr);
    ~Snake();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onTimeout();
    void onMoveTimeout();
    void onPlayClicked();
    void onExitClicked();
    void onHighScoreClicked();

private:
    static const int BOARD_WIDTH = 20, BOARD_HEIGHT = 20, MOVE_INTERVAL = 100, TIMEOUT_SECONDS = 5, INIT_LENGTH = 4, UNIT_SIZE = 20;
    enum Direction { Left, Right, Up, Down };
    enum GameState { Menu, Game, Over, ShowHighScore };
    struct Point {
        int x;
        int y;
    };

    void startGame();
    void move();
    void checkCollision();
    void placeFood();
    void drawBoard(QPainter &qp);
    void drawMenu(QPainter &qp);
    void drawGameOver(QPainter &qp);
    void drawHighScore(QPainter &qp);
    void gameOver();
    void reset();
    void loadHighScore();
    void saveHighScore();

    QTimer *gameTimer;
    QTimer *moveTimer;
    QPushButton *playButton;
    QPushButton *exitButton;
    QPushButton *highScoreButton;
    QVector<Point> snake;
    Point food;
    Direction dir;
    GameState inGame;
    int score;
    int highScore;
    int timeRemaining;
};

#endif
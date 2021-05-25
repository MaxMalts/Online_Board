#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QShowEvent>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QAction>
#include <QActionGroup>
#include <QToolButton>

#include "canvas.h"
#include "ServerApi/serverapi.h"
#include "common.h"


QT_BEGIN_NAMESPACE
namespace Ui { class OnlineBoard; }
QT_END_NAMESPACE


class OnlineBoard : public QMainWindow
{
    Q_OBJECT

private:
    const QSize init_size = QSize(500, 500);

public:
    OnlineBoard(QWidget* parent = nullptr);
    ~OnlineBoard();

protected:
    void resizeEvent(QResizeEvent*) override;
    void keyPressEvent(QKeyEvent* event);

private slots:
    void onServerDisconnected();
    void onServerConnected();
    void onFinishBoardInit();

    void onToolsActionTriggered(QAction* action);
    void onToolButtonClicked();
    void onToolButtonTriggered(QAction* action);
    void onFigureButtonRClick();
    void onFiguresFocusOut();

    void onColorButtonClicked();
    void onUndoTriggered();
    void onClearTriggered();
    void onFullscreenTriggered(bool checked);

private:
    void InitTools();

    Canvas::ToolType selected_figure = Canvas::ToolType::rectangle;
    void handleFigureButtonClicked();
    void handleFigureButtonTriggered(QToolButton* button);

    Ui::OnlineBoard* ui;

    QActionGroup* tools_actions;
    BiMap<Canvas::ToolType, QAction*> tool_type_to_action;

    Canvas* canvas = nullptr;
};
#endif // MAINWINDOW_H

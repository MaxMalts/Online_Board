#include <QWidget>
#include <QShowEvent>
#include <QResizeEvent>
#include <QAction>
#include <QActionGroup>
#include <QToolButton>
#include <QLayout>
#include <QString>
#include <QSet>
#include <QSizePolicy>
#include <QColor>
#include <QColorDialog>
#include <QDebug>

#include "onlineboard.h"
#include "ui_onlineboard.h"
#include "canvas.h"
#include "common.h"


OnlineBoard::OnlineBoard(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::OnlineBoard)
{
    setEnabled(false);  // waiting for board initialization

    ui->setupUi(this);

    canvas = new Canvas(QSize(0, 0), centralWidget());
    canvas->lower();
    canvas->setSizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Ignored);
    canvas->setFocusPolicy(Qt::ClickFocus);

    InitTools();

    connect(ServerApi::getInstance(), SIGNAL(disconnected()),
            this, SLOT(onServerDisconnected()));
    connect(ServerApi::getInstance(), SIGNAL(connected()),
            this, SLOT(onServerConnected()));
    connect(ServerApi::getInstance(), SIGNAL(cFinishBoardInit(const Serializer&)),
            this, SLOT(onFinishBoardInit()));

    if (!ServerApi::connectToServer()) {
        statusBar()->setStyleSheet("color: red;");
        statusBar()->showMessage("Disconnected from server: " +
                                 ServerApi::lastErrorStr() + ". Reconnecting...");

        qDebug() << "Error while connecting to server: " << ServerApi::lastError();

    } else {
        statusBar()->setStyleSheet("");
        statusBar()->showMessage("Connected to server.", 3000);
    }
}

void OnlineBoard::InitTools()
{
    tool_type_to_action = BiMap<Canvas::ToolType, QAction*> {
        { Canvas::ToolType::pencil, ui->actionPencil },
        { Canvas::ToolType::line, ui->actionLine },
        { Canvas::ToolType::rectangle, ui->actionRectangle },
        { Canvas::ToolType::ellipse, ui->actionEllipse }
    };

    tools_actions = new QActionGroup(ui->menuTools);

    tools_actions->addAction(ui->actionPencil);
    tools_actions->addAction(ui->actionLine);
    tools_actions->addAction(ui->actionRectangle);
    tools_actions->addAction(ui->actionEllipse);
    connect(tools_actions, SIGNAL(triggered(QAction*)),
            this, SLOT(onToolsActionTriggered(QAction*)));

    ui->buttonPencil->addAction(ui->actionPencil);
    ui->buttonLine->addAction(ui->actionLine);
    ui->buttonRectangle->addAction(ui->actionRectangle);
    ui->buttonEllipse->addAction(ui->actionEllipse);

    EventSignalAdapter* figure_adapter =
            new EventSignalAdapter(ui->buttonFigure);
    connect(figure_adapter, SIGNAL(rClick()),
            this, SLOT(onFigureButtonRClick()));

    EventSignalAdapter* figures_adapter =
            new EventSignalAdapter(ui->figures);
    connect(figures_adapter, SIGNAL(focusOut()),
            this, SLOT(onFiguresFocusOut()));

    ui->figures->hide();
}

void OnlineBoard::resizeEvent(QResizeEvent*)
{
    QSize canvas_size(size().width(),
                      size().height() - statusBar()->height() -
                      menuBar()->height() + 1);

    canvas->resize(canvas_size);
}

void OnlineBoard::onServerDisconnected()
{
    statusBar()->setStyleSheet("color: red;");
    statusBar()->showMessage("Disconnected from server: " +
                             ServerApi::lastErrorStr() + " Reconnecting...");
    setEnabled(false);

    qDebug() << "Disconnected from server: " << ServerApi::lastError();
}


void OnlineBoard::onServerConnected()
{
    statusBar()->setStyleSheet("");
    statusBar()->showMessage("Connected to server.", 3000);
}

void OnlineBoard::onFinishBoardInit()
{
    qDebug() << "Board initialized.";
    setEnabled(true);
}

OnlineBoard::~OnlineBoard()
{
    delete ui;
}


/* User interaction handlers */

void OnlineBoard::onToolsActionTriggered(QAction* action)
{
    Canvas::ToolType tool = tool_type_to_action.atR(action);
    canvas->setActiveTool(tool);
}

void OnlineBoard::handleFigureButtonClicked()
{
    QToolButton* selected_tool_button = nullptr;
    if (selected_figure == Canvas::ToolType::rectangle) {
        selected_tool_button = ui->buttonRectangle;
    } else {
        Q_ASSERT(selected_figure == Canvas::ToolType::ellipse);
        selected_tool_button = ui->buttonEllipse;
    }

    Q_ASSERT(selected_tool_button->actions().size() == 1);
    selected_tool_button->actions().at(0)->trigger();
}

void OnlineBoard::handleFigureButtonTriggered(QToolButton* button)
{
    if (button == ui->buttonRectangle) {
        selected_figure = Canvas::ToolType::rectangle;
    } else {
        Q_ASSERT(button == ui->buttonEllipse);
        selected_figure = Canvas::ToolType::ellipse;
    }

    ui->buttonFigure->setIcon(button->icon());
    ui->buttonFigure->setChecked(true);
    ui->figures->clearFocus();
}

void OnlineBoard::onToolButtonClicked()
{
    QToolButton* button = dynamic_cast<QToolButton*>(sender());

    if (button == ui->buttonFigure) {
        handleFigureButtonClicked();

    } else {
        Q_ASSERT(button->actions().size() == 1);

        button->actions().at(0)->trigger();
    }
}

void OnlineBoard::onToolButtonTriggered(QAction* action)
{
    QToolButton* button = dynamic_cast<QToolButton*>(sender());
    Q_ASSERT(button->actions().size() == 1);
    Q_ASSERT(button->actions().at(0) == action);

    if (button == ui->buttonRectangle ||
        button == ui->buttonEllipse) {

        handleFigureButtonTriggered(button);

    } else {
        button->setChecked(true);
    }
}

void OnlineBoard::onFigureButtonRClick()
{
    ui->figures->show();
    ui->figures->setFocus();
}

void OnlineBoard::onFiguresFocusOut()
{
    ui->figures->hide();
}


void OnlineBoard::onColorButtonClicked()
{
    Q_ASSERT(sender() == ui->colorButton);

    QColor new_color = QColorDialog::getColor(canvas->activeColor(), this,
                                              "Color Picker",
                                              QColorDialog::ShowAlphaChannel);
    if (!new_color.isValid()) {
        return;
    }

    QString css = ui->colorButton->styleSheet();
    int replace_pos = css.indexOf(":", css.indexOf("background-color:")) + 1;
    int replace_size = css.indexOf(";", replace_pos) - replace_pos;
    ui->colorButton->setStyleSheet(css.replace(replace_pos, replace_size,
                                               new_color.name(QColor::HexArgb)));

    qDebug().noquote() << ui->colorButton->styleSheet();
    qDebug() << new_color;

    canvas->setActiveColor(new_color);
}

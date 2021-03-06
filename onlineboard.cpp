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
#include <QMessageBox>
#include <QDebug>

#include "onlineboard.h"
#include "ui_onlineboard.h"
#include "canvas.h"
#include "common.h"


OnlineBoard::OnlineBoard(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::OnlineBoard)
{
    ui->setupUi(this);
    centralWidget()->setEnabled(false);  // waiting for board initialization

    canvas = new Canvas(QSize(0, 0), centralWidget());
    canvas->setupTools(ui->toolsPropsContainer->layout());
    canvas->setActiveTool(Canvas::ToolType::hand);
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

    setWindowState(windowState().setFlag(Qt::WindowFullScreen, true));
}

void OnlineBoard::InitTools()
{
    tool_type_to_action = BiMap<Canvas::ToolType, QAction*> {
        { Canvas::ToolType::hand, ui->actionHand },
        { Canvas::ToolType::pencil, ui->actionPencil },
        { Canvas::ToolType::line, ui->actionLine },
        { Canvas::ToolType::rectangle, ui->actionRectangle },
        { Canvas::ToolType::ellipse, ui->actionEllipse }
    };

    tools_actions = new QActionGroup(ui->menuTools);

    tools_actions->addAction(ui->actionHand);
    tools_actions->addAction(ui->actionPencil);
    tools_actions->addAction(ui->actionLine);
    tools_actions->addAction(ui->actionRectangle);
    tools_actions->addAction(ui->actionEllipse);
    connect(tools_actions, SIGNAL(triggered(QAction*)),
            this, SLOT(onToolsActionTriggered(QAction*)));

    ui->buttonHand->addAction(ui->actionHand);
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

    if (canvas != nullptr)
        canvas->resize(canvas_size);
}

void OnlineBoard::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape &&
        ui->actionFullscreen->isChecked()) {

        ui->actionFullscreen->trigger();
    }
}

void OnlineBoard::onServerDisconnected()
{
    statusBar()->setStyleSheet("color: red;");
    statusBar()->showMessage("Disconnected from server: " +
                             ServerApi::lastErrorStr() + " Reconnecting...");
    centralWidget()->setEnabled(false);

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
    centralWidget()->setEnabled(true);
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
    Q_ASSERT(sender() == ui->buttonColor);

    QColor new_color = QColorDialog::getColor(canvas->activeColor(), this,
                                              "Color Picker",
                                              QColorDialog::ShowAlphaChannel);
    if (!new_color.isValid()) {
        return;
    }

    QString new_css = ui->colorPreview->styleSheet();
    bool ret = ChangeCssProp(new_css, "background",
                             new_color.name(QColor::HexArgb));
    Q_ASSERT(ret);
    ui->colorPreview->setStyleSheet(new_css);

    new_css = ui->colorPreviewBgd->styleSheet();
    ret = ChangeCssProp(new_css, "border-color",
                        new_color.darker(150).name(QColor::HexRgb));
    Q_ASSERT(ret);
    ui->colorPreviewBgd->setStyleSheet(new_css);

    canvas->setActiveColor(new_color);
}

void OnlineBoard::onUndoTriggered()
{
    canvas->undo();
}

void OnlineBoard::onClearTriggered()
{
    QMessageBox confirm_dialog;
    confirm_dialog.setText("Clear board?");
    confirm_dialog.setInformativeText("This action can't be undone!");
    confirm_dialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirm_dialog.setDefaultButton(QMessageBox::No);
    confirm_dialog.setFixedWidth(1000);

    if (confirm_dialog.exec() == QMessageBox::Yes) {
        canvas->clear();
    }
}

void OnlineBoard::onFullscreenTriggered(bool checked)
{
    if (checked) {
        setWindowState(windowState().setFlag(Qt::WindowFullScreen, true));
    } else {
        setWindowState(windowState().setFlag(Qt::WindowFullScreen, false));
    }
}

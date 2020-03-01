#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QtConcurrent>
#include <QStringList>
#include <QHash>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QCryptographicHash>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	
private:
	Ui::MainWindow *ui;
	QString	  m_currentTrahDir  = "";  //存放重复文件目录
	QString     m_currentDir = "/"; //当前选择的相册路径
	QString     m_uiResult;
	QStringList m_currentFileList; //记录当前已经遍历了的文件集合
	int		  m_currentFindRepeat = 0; //扫描室记录重复文件
	int         m_progressBarValue = 0;
	QHash<QByteArray,QString> map;  //存放所有图片路径
	QHash<QString,QString> filterMap;  //存在重复图片路径
private slots:
	void getDir();  //获取相册目录
	void getTrashDir(); //重复存放重复相册的目录
	void scanPhoto();  //扫描所有图片文件
	void filter();    //过滤重复图片
	void addToStorage(QString fileName);
	
	QByteArray getMd5(QString fileName);
	QStringList getAllFile(QString Dirpath);  //遍历获取
	
signals:
	void updateTextEdit();
	void updateLabel();
	void updateProgressBar();
};

#endif // MAINWINDOW_H

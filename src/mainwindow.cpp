#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->label->setText(tr("已发现重复文件:"));
	this->setWindowTitle(tr("Find Repeat"));
	connect(ui->ButtonChoese,&QPushButton::clicked,this,&MainWindow::getDir);
	connect(ui->ButtonScan,&QPushButton::clicked,this,&MainWindow::scanPhoto);
	connect(ui->ButtonChoeseTrash,&QPushButton::clicked,this,&MainWindow::getTrashDir);
	connect(ui->ButtonFilter,&QPushButton::clicked,this,&MainWindow::filter);
	connect(this,&MainWindow::updateTextEdit,this,[=]{
			
			QHash<QString,QString>::Iterator it;
			for(it =filterMap.begin();it != filterMap.end();++it)
				{
					ui->textEdit->append(it.key() +" "+ it.value());
				}
			ui->progressBar->setMaximum(m_currentFindRepeat);
		});
	
	connect(this,&MainWindow::updateLabel,this,[=](){
			m_currentFindRepeat = m_currentFindRepeat+1;
			this->ui->label->setText(tr("已发现重复文件:")+QString::number(m_currentFindRepeat));
 		});
	
	connect(this,&MainWindow::updateProgressBar,this,[=](){
			m_progressBarValue = m_progressBarValue+1;
			ui->progressBar->setValue(m_progressBarValue);
		});
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::getDir()
{
	m_currentDir = QFileDialog::getExistingDirectory(this, 
										    tr("Open Directory"),
										    m_currentDir,
										    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	//
	qDebug()<<"choese "<<m_currentDir;
}

void MainWindow::getTrashDir()
{
	QMessageBox::information(this,tr("提示"),tr("请不要将存放重复文件的目录放到要扫描的目录下面\n 比如h:/down是我们要扫描的目录,那就不要选择down下面的目录作为存放重复文件的目录"));
	m_currentTrahDir = QFileDialog::getExistingDirectory(this, 
											   tr("Open Directory"),
											   m_currentTrahDir,
											   QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	//
	qDebug()<<"choese "<<m_currentTrahDir;
}

void MainWindow::scanPhoto()
{
	m_currentFindRepeat = 0;
	map.clear();
	filterMap.clear();
	m_currentFileList.clear();
	QtConcurrent::run([=](){
			getAllFile(m_currentDir);
			emit this->updateTextEdit();
		});
}

void MainWindow::filter()
{
	if(m_currentTrahDir.isEmpty())
		{
			QMessageBox::information(this,tr("警告"),tr("请选择存放重复文件的目录"));
			return;
		}
	QtConcurrent::run([=](){
			QHash<QString, QString>::const_iterator i = filterMap.constBegin();
			while (i != filterMap.constEnd()) {
					//下面移动到重复相册
					QFileInfo info(i.key());
					QFile::rename(i.key(),m_currentTrahDir+"/"+info.fileName());
					this->updateProgressBar();
					++i;
				}
			
		});
}

void MainWindow::addToStorage(QString fileName)
{
	QHash<QByteArray,QString>::Iterator it;
	QByteArray md5Hex = getMd5(fileName);
	it = map.find(md5Hex);
	
	while(it != map.end() && it.key()==md5Hex)
		{
			this->updateLabel();
			filterMap.insert(fileName,it.value());
			++it;
		}
	map.insert(md5Hex,fileName);
}

QByteArray MainWindow::getMd5(QString fileName)
{
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly))
		{
			qDebug()<<"read onli fail "<<file.errorString();
			return QByteArray();
		}
	QByteArray buff = file.readAll();
	file.close();
	return QCryptographicHash::hash(buff,QCryptographicHash::Md5).toHex();
}

QStringList MainWindow::getAllFile(QString Dirpath)
{
	qDebug()<<"entry Dir "<<Dirpath;
	QStringList filePathNames;
	QDir splDir(Dirpath);
	QFileInfoList fileInfoListInSplDir = splDir.entryInfoList(QDir::NoDotAndDotDot |QDir::Files | QDir::Dirs);
	QFileInfo tempFileInfo;
	for(int i=0;i<fileInfoListInSplDir.size();i++)
		{
			tempFileInfo = fileInfoListInSplDir.at(i);
			
			if(tempFileInfo.isFile())
				{
					qDebug()<<"file ";
					qDebug()<<tempFileInfo.absoluteFilePath();
					addToStorage(tempFileInfo.absoluteFilePath());
					m_currentFileList << tempFileInfo.absoluteFilePath();
				}
			else if(tempFileInfo.isDir())
				{
					qDebug()<<"dir ";
					qDebug()<<tempFileInfo.absoluteFilePath();
					filePathNames << getAllFile(tempFileInfo.absoluteFilePath());
				}
			else{
					qDebug()<<"nothing ";
					qDebug()<<tempFileInfo.fileName();
				}
		}
	
	return filePathNames;
	
}

#include "topblockmanager.h"
#include <logging_categories.h>

TopBlockManager::TopBlockManager(QObject *parent) : QObject(parent) {

}

TopBlockManager::~TopBlockManager() {

}

void TopBlockManager::add(QString name)
{
//	top = new TopBlock(("IIOManager " + name).toStdString());

	qDebug(CAT_IIO_MANAGER()) << "Added to TopBlockManager: " + name;
	topblocks[name.toStdString().c_str()] = boost::make_shared<TopBlock>(name.toStdString());

}

boost::shared_ptr<TopBlock> TopBlockManager::get(QString name)
{
	return topblocks[name];
}

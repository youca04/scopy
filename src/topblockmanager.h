#ifndef TOPBLOCKMANAGER_H
#define TOPBLOCKMANAGER_H

#include <QObject>
#include <QString>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <topblock.h>

class TopBlockManager : public QObject
{
	Q_OBJECT
public:
	TopBlockManager(QObject *parent = nullptr);
	~TopBlockManager();

	void add(QString name);
	boost::shared_ptr<TopBlock> get(QString name);
	// void remove


private:
	std::map<QString, boost::shared_ptr<TopBlock>> topblocks;
};

#endif // TOPBLOCKMANAGER_H

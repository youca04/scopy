#include "topblock.h"
#include <QDebug>
#include <logging_categories.h>

TopBlock::TopBlock(std::string name, QObject *parent) : QObject(parent), gr::top_block(name)
{
	this->name = name;

	qDebug(CAT_IIO_MANAGER) <<  "Created top block " + QString::fromStdString(this->name);
}


TopBlock::~TopBlock() {

	qDebug(CAT_IIO_MANAGER) <<  "Destroyed top block " + QString::fromStdString(this->name);

}


void TopBlock::addSource(gr::basic_block_sptr blk)
{
	sources.push_back(blk);
}

void TopBlock::replaceBlock(gr::basic_block_sptr src, gr::basic_block_sptr dst)
{
	unsigned int i=0;

	for(i = 0; i < connections.size(); i++) {
		if(connections[i].src == src) {
			hier_block2::disconnect(connections[i].src, connections[i].src_port, connections[i].dst, connections[i].dst_port);
			hier_block2::connect(dst, connections[i].src_port, connections[i].dst, connections[i].dst_port);
			connections[i].src = dst;
		}
		if(connections[i].dst == src) {
			hier_block2::disconnect(connections[i].src, connections[i].src_port, connections[i].dst, connections[i].dst_port);
			hier_block2::connect(connections[i].src, connections[i].src_port, dst, connections[i].dst_port);
			connections[i].dst = src;
		}
	}
	for(unsigned int i=0;i< sources.size();i++) {
		if(sources[i] == src)
			sources[i] = dst;
	}

}

std::vector<gr::basic_block_sptr> TopBlock::getSources() const
{
	return sources;
}

unsigned int TopBlock::getNrOfSources() const
{
	return sources.size();
}


void TopBlock::connect(gr::basic_block_sptr src, int src_port,
		gr::basic_block_sptr dst, int dst_port)
{
	struct connection entry;
	entry.src = src;
	entry.dst = dst;
	entry.src_port = src_port;
	entry.dst_port = dst_port;

	connections.push_back(entry);
	hier_block2::connect(src, src_port, dst, dst_port);
}

void TopBlock::disconnect(gr::basic_block_sptr src, int src_port,
		gr::basic_block_sptr dst, int dst_port)
{
	for (auto it = connections.begin(); it != connections.end(); ++it) {
		if (it->src == src && it->dst == dst &&
				it->src_port == src_port &&
				it->dst_port == dst_port) {
			connections.erase(it);
			break;
		}
	}

	hier_block2::disconnect(src, src_port, dst, dst_port);
}

void TopBlock::del_connection(gr::basic_block_sptr block, bool reverse)
{
	bool found;

	do {
		found = false;

		for (auto it = connections.begin();
				it != connections.end(); ++it) {
			if (reverse) {
				if (block != it->dst)
					continue;
			} else if (block != it->src) {
				continue;
			}

			qDebug(CAT_IIO_MANAGER) << "Removing" <<
				(reverse ? "backwards" : "forward")
				<< "connection between"
				<< it->src->alias().c_str()
				<< "port" << it->src_port << "and"
				<< it->dst->alias().c_str()
				<< "port" << it->dst_port;
			hier_block2::disconnect(it->src, it->src_port,
					it->dst, it->dst_port);

			auto src = it->src, dst = it->dst;
			connections.erase(it);
			if (reverse)
				del_connection(src, true);
			else
				del_connection(dst, true);
			found = true;
			break;
		}
	} while (found);

	if (reverse)
		del_connection(block, false);
}


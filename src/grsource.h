#ifndef GRSOURCE_H
#define GRSOURCE_H

#include <QObject>
#include <gnuradio/hier_block2.h>
#include <iio.h>


class GRSource : public QObject, virtual public gr::hier_block2
{
	Q_OBJECT
protected:
	iio_context *iio;
	unsigned int nb_channels;
	unsigned long buffer_size;

public:

	typedef boost::shared_ptr<GRSource> sptr;

	GRSource(struct iio_context *ctx, QObject *parent = nullptr);
	virtual ~GRSource();
	//static sptr make(struct iio_context *ctx);


	virtual void set_buffer_size(unsigned long size);
	virtual void update_buffer_size_unlocked() = 0;

	virtual void set_device_timeout(unsigned int mseconds);

private Q_SLOTS:
	void got_timeout();

Q_SIGNALS:
	void timeout();

};


#endif // GRSOURCE_H

#ifndef IIOSOURCE_H
#define IIOSOURCE_H


#include <iio/device_source.h>
#include <gnuradio/hier_block2.h>
#include <frequency_compensation_filter.h>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/analog/m2kanalogin.hpp>
#include <timeout_block.hpp>
#include <grsource.h>

/* 1k samples by default */
#define IIO_BUFFER_SIZE 0x400

class IioSource : public GRSource
{
	Q_OBJECT
private:

	iio_context *ctx;
	gr::iio::device_source::sptr iio_blk;

	adiscope::timeout_block::sptr timeout_b;

public:
	typedef boost::shared_ptr<IioSource> sptr;

	IioSource(struct iio_context *ctx, std::string _dev, QObject *parent = nullptr);
	~IioSource();
	static sptr make(struct iio_context *ctx, std::string _dev);


	void set_buffer_size(unsigned long size);
	void update_buffer_size_unlocked();

	void set_device_timeout(unsigned int mseconds);

Q_SIGNALS:
	void timeout();

};
#endif // IIOSOURCE_H

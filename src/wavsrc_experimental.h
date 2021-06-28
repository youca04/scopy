#ifndef WAVSRC_EXPERIMENTAL_H
#define WAVSRC_EXPERIMENTAL_H

#include <iio/device_source.h>
#include <gnuradio/hier_block2.h>
#include <frequency_compensation_filter.h>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/analog/m2kanalogin.hpp>
#include <timeout_block.hpp>
#include <grsource.h>
#include <gnuradio/blocks/wavfile_source.h>

/* 1k samples by default */
#define IIO_BUFFER_SIZE 0x400

class WavSrc_Experimental : public GRSource
{
	Q_OBJECT
private:

	iio_context *ctx;
	//gr::iio::device_source::sptr iio_blk;
	gr::blocks::wavfile_source::sptr iio_blk;
	adiscope::timeout_block::sptr timeout_b;

public:
	typedef boost::shared_ptr<WavSrc_Experimental> sptr;

	WavSrc_Experimental(struct iio_context *ctx, std::string _dev, QObject *parent = nullptr);
	~WavSrc_Experimental();
	static sptr make(struct iio_context *ctx, std::string _dev);


	void set_buffer_size(unsigned long size);
	void update_buffer_size_unlocked();

	void set_device_timeout(unsigned int mseconds);

Q_SIGNALS:
	void timeout();

};

#endif // WAVSRC_EXPERIMENTAL_H

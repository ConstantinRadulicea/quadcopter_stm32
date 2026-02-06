#include "msp.h"
#include "stdint.h"


// timeout in milliseconds
int8_t recv(msp_t *msp, uint8_t * messageID, void * payload, uint8_t maxSize, uint8_t * recvSize) {
  uint32_t t0 = msp->sys_now_ms();

  while (1) {

    // read header
    while (_stream->available() < 6)
      if (msp->sys_now_ms() - t0 >= msp->_timeout_ms)
        return 0;
    char header[3];
    _stream->readBytes((char*)header, 3);

    // check header
    if (header[0] == '$' && header[1] == 'M' && header[2] == '<') {
      // header ok, read payload size
      *recvSize = _stream->read();

      // read message ID (type)
      *messageID = _stream->read();

      uint8_t checksumCalc = *recvSize ^ *messageID;

      // read payload
      uint8_t * payloadPtr = (uint8_t*)payload;
      uint8_t idx = 0;
      while (idx < *recvSize) {
        if (msp->sys_now_ms() - t0 >= msp->_timeout_ms)
          return 0;
        if (_stream->available() > 0) {
          uint8_t b = _stream->read();
          checksumCalc ^= b;
          if (idx < maxSize)
            *(payloadPtr++) = b;
          ++idx;
        }
      }
      // zero remaining bytes if *size < maxSize
      for (; idx < maxSize; ++idx)
        *(payloadPtr++) = 0;

      // read and check checksum
      while (_stream->available() == 0)
        if (msp->sys_now_ms() - t0 >= msp->_timeout_ms)
          return 0;
      uint8_t checksum = _stream->read();
      if (checksumCalc == checksum) {
        return 1;
      }

    }
  }

}


static int8_t msp_receive_frame(msp_t *msp, uint8_t rxByte){
	uint8_t framePosition = msp->rx_frame_position;
	uint32_t frameStartTime = msp->rx_frame_start_time_ms;
	uint32_t currentTime = msp->sys_now_ms();
	uint32_t fullFrameLength = 0;

	/* Reset the frame position if the frame time has expired. */
	if (currentTime - frameStartTime > msp->_timeout_ms) {
		framePosition = 0;

		if (currentTime < frameStartTime) {
			frameStartTime = currentTime;
			msp->rx_frame_start_time_ms = frameStartTime;
		}
	}

	if (framePosition == 0) {
		frameStartTime = currentTime;
		msp->rx_frame_start_time_ms = frameStartTime;
	}

	return 0;
}

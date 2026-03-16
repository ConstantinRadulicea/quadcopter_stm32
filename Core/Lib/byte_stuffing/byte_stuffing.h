#ifndef BYTE_STUFFING_H
#define BYTE_STUFFING_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BYTE_STUFFING_PPP_FLAG			(0x7E)
#define BYTE_STUFFING_PPP_ESCAPE		(0x7D)
#define BYTE_STUFFING_PPP_ESC_MASK 		(0x20)


/**
 * @brief Escapes a single byte according to HDLC/PPP byte stuffing rules.
 * * If the input byte matches the flag byte or the escape byte itself,
 * it writes a two-byte sequence (the escape byte followed by the
 * XOR-masked original byte) to the output buffer. Otherwise, it
 * writes the raw byte.
 * * @param byte               The raw input byte to evaluate.
 * @param out_buf            Pointer to the destination buffer.
 * @param out_len_remaining  Available space left in the destination buffer.
 * @param escaped_byte       The flag byte to avoid (e.g., 0x7E).
 * @param escape_byte        The control byte used to indicate escaping (e.g., 0x7D).
 * @param escape_mask        The mask used to XOR the data byte (e.g., 0x20).
 * * @return int               The number of bytes written to out_buf (1 or 2).
 * Returns 0 if there is insufficient space (buffer overflow).
 */
int escape_byte_(
        uint8_t byte,
        uint8_t* out_buf,
        size_t out_len_remaining,
        uint8_t escaped_byte,
        uint8_t escape_byte,
        uint8_t escape_mask
        );

/**
 * @brief Escapes an entire data payload for serial transmission.
 * * Iterates through the input buffer and applies byte stuffing to guarantee
 * that the control flags (like 0x7E) never appear in the output stream.
 * * @param in_buf             Pointer to the raw data payload.
 * @param in_len             Number of bytes in the raw payload.
 * @param out_buf            Pointer to the destination buffer for the framed data.
 * @param max_out_len        Maximum capacity of the destination buffer.
 * @param escaped_byte       The flag byte to avoid (e.g., 0x7E).
 * @param escape_byte        The control byte used to indicate escaping (e.g., 0x7D).
 * @param escape_mask        The mask used to XOR the data byte (e.g., 0x20).
 * * @return size_t            The total number of bytes written to the output buffer.
 * Returns (max_out_len + 1) on error (e.g., output buffer overflow or 0-length input).
 */
size_t escape_buffer_(
        const uint8_t* in_buf,
        size_t in_len,
        uint8_t* out_buf,
        size_t max_out_len,
        uint8_t escaped_byte,
        uint8_t escape_byte,
        uint8_t escape_mask
        );

/**
 * @brief Removes HDLC/PPP byte stuffing from a received frame body.
 * * Processes an incoming framed payload, dropping the escape characters
 * and using the XOR mask to restore the original data bytes.
 * Note: This function assumes the start/end flag bytes (0x7E) have
 * already been removed from the input buffer.
 * * @param in_buf             Pointer to the received escaped data.
 * @param in_len             Number of bytes in the received data.
 * @param out_buf            Pointer to the destination buffer for the clean data.
 * @param max_out_len        Maximum capacity of the destination buffer.
 * @param escaped_byte       The flag byte (e.g., 0x7E) - typically unused here but kept for API symmetry.
 * @param escape_byte        The control byte that indicates the next byte is masked (e.g., 0x7D).
 * @param escape_mask        The mask used to restore the original data byte (e.g., 0x20).
 * * @return size_t            The number of clean, unescaped bytes written to out_buf.
 * Returns (max_out_len + 1) on error (buffer overflow or a dangling escape byte at the end).
 */
size_t unescape_buffer_(
        const uint8_t* in_buf,
        size_t in_len,
        uint8_t* out_buf,
        size_t max_out_len,
        uint8_t escaped_byte,
        uint8_t escape_byte,
        uint8_t escape_mask
        );





#ifdef __cplusplus
}
#endif

#endif

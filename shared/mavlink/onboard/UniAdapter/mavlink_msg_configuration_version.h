// MESSAGE CONFIGURATION_VERSION PACKING

#define MAVLINK_MSG_ID_CONFIGURATION_VERSION 181

typedef struct __mavlink_configuration_version_t
{
 uint16_t fw_version; ///< Firmware Version
 uint8_t port1; ///< Protocol used on Port 1
 uint8_t port2; ///< Protocol used on Port 2
} mavlink_configuration_version_t;

#define MAVLINK_MSG_ID_CONFIGURATION_VERSION_LEN 4
#define MAVLINK_MSG_ID_181_LEN 4

#define MAVLINK_MSG_ID_CONFIGURATION_VERSION_CRC 61
#define MAVLINK_MSG_ID_181_CRC 61



#define MAVLINK_MESSAGE_INFO_CONFIGURATION_VERSION { \
	"CONFIGURATION_VERSION", \
	3, \
	{  { "fw_version", NULL, MAVLINK_TYPE_UINT16_T, 0, 0, offsetof(mavlink_configuration_version_t, fw_version) }, \
         { "port1", NULL, MAVLINK_TYPE_UINT8_T, 0, 2, offsetof(mavlink_configuration_version_t, port1) }, \
         { "port2", NULL, MAVLINK_TYPE_UINT8_T, 0, 3, offsetof(mavlink_configuration_version_t, port2) }, \
         } \
}


/**
 * @brief Pack a configuration_version message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param fw_version Firmware Version
 * @param port1 Protocol used on Port 1
 * @param port2 Protocol used on Port 2
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_configuration_version_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       uint16_t fw_version, uint8_t port1, uint8_t port2)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_CONFIGURATION_VERSION_LEN];
	_mav_put_uint16_t(buf, 0, fw_version);
	_mav_put_uint8_t(buf, 2, port1);
	_mav_put_uint8_t(buf, 3, port2);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_CONFIGURATION_VERSION_LEN);
#else
	mavlink_configuration_version_t packet;
	packet.fw_version = fw_version;
	packet.port1 = port1;
	packet.port2 = port2;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_CONFIGURATION_VERSION_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_CONFIGURATION_VERSION;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_CONFIGURATION_VERSION_LEN, MAVLINK_MSG_ID_CONFIGURATION_VERSION_CRC);
#else
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_CONFIGURATION_VERSION_LEN);
#endif
}

/**
 * @brief Pack a configuration_version message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param fw_version Firmware Version
 * @param port1 Protocol used on Port 1
 * @param port2 Protocol used on Port 2
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_configuration_version_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           uint16_t fw_version,uint8_t port1,uint8_t port2)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_CONFIGURATION_VERSION_LEN];
	_mav_put_uint16_t(buf, 0, fw_version);
	_mav_put_uint8_t(buf, 2, port1);
	_mav_put_uint8_t(buf, 3, port2);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_CONFIGURATION_VERSION_LEN);
#else
	mavlink_configuration_version_t packet;
	packet.fw_version = fw_version;
	packet.port1 = port1;
	packet.port2 = port2;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_CONFIGURATION_VERSION_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_CONFIGURATION_VERSION;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_CONFIGURATION_VERSION_LEN, MAVLINK_MSG_ID_CONFIGURATION_VERSION_CRC);
#else
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_CONFIGURATION_VERSION_LEN);
#endif
}

/**
 * @brief Encode a configuration_version struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param configuration_version C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_configuration_version_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_configuration_version_t* configuration_version)
{
	return mavlink_msg_configuration_version_pack(system_id, component_id, msg, configuration_version->fw_version, configuration_version->port1, configuration_version->port2);
}

/**
 * @brief Encode a configuration_version struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param configuration_version C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_configuration_version_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_configuration_version_t* configuration_version)
{
	return mavlink_msg_configuration_version_pack_chan(system_id, component_id, chan, msg, configuration_version->fw_version, configuration_version->port1, configuration_version->port2);
}

/**
 * @brief Send a configuration_version message
 * @param chan MAVLink channel to send the message
 *
 * @param fw_version Firmware Version
 * @param port1 Protocol used on Port 1
 * @param port2 Protocol used on Port 2
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_configuration_version_send(mavlink_channel_t chan, uint16_t fw_version, uint8_t port1, uint8_t port2)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_CONFIGURATION_VERSION_LEN];
	_mav_put_uint16_t(buf, 0, fw_version);
	_mav_put_uint8_t(buf, 2, port1);
	_mav_put_uint8_t(buf, 3, port2);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_VERSION, buf, MAVLINK_MSG_ID_CONFIGURATION_VERSION_LEN, MAVLINK_MSG_ID_CONFIGURATION_VERSION_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_VERSION, buf, MAVLINK_MSG_ID_CONFIGURATION_VERSION_LEN);
#endif
#else
	mavlink_configuration_version_t packet;
	packet.fw_version = fw_version;
	packet.port1 = port1;
	packet.port2 = port2;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_VERSION, (const char *)&packet, MAVLINK_MSG_ID_CONFIGURATION_VERSION_LEN, MAVLINK_MSG_ID_CONFIGURATION_VERSION_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_VERSION, (const char *)&packet, MAVLINK_MSG_ID_CONFIGURATION_VERSION_LEN);
#endif
#endif
}

#if MAVLINK_MSG_ID_CONFIGURATION_VERSION_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_configuration_version_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint16_t fw_version, uint8_t port1, uint8_t port2)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_uint16_t(buf, 0, fw_version);
	_mav_put_uint8_t(buf, 2, port1);
	_mav_put_uint8_t(buf, 3, port2);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_VERSION, buf, MAVLINK_MSG_ID_CONFIGURATION_VERSION_LEN, MAVLINK_MSG_ID_CONFIGURATION_VERSION_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_VERSION, buf, MAVLINK_MSG_ID_CONFIGURATION_VERSION_LEN);
#endif
#else
	mavlink_configuration_version_t *packet = (mavlink_configuration_version_t *)msgbuf;
	packet->fw_version = fw_version;
	packet->port1 = port1;
	packet->port2 = port2;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_VERSION, (const char *)packet, MAVLINK_MSG_ID_CONFIGURATION_VERSION_LEN, MAVLINK_MSG_ID_CONFIGURATION_VERSION_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_VERSION, (const char *)packet, MAVLINK_MSG_ID_CONFIGURATION_VERSION_LEN);
#endif
#endif
}
#endif

#endif

// MESSAGE CONFIGURATION_VERSION UNPACKING


/**
 * @brief Get field fw_version from configuration_version message
 *
 * @return Firmware Version
 */
static inline uint16_t mavlink_msg_configuration_version_get_fw_version(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint16_t(msg,  0);
}

/**
 * @brief Get field port1 from configuration_version message
 *
 * @return Protocol used on Port 1
 */
static inline uint8_t mavlink_msg_configuration_version_get_port1(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  2);
}

/**
 * @brief Get field port2 from configuration_version message
 *
 * @return Protocol used on Port 2
 */
static inline uint8_t mavlink_msg_configuration_version_get_port2(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  3);
}

/**
 * @brief Decode a configuration_version message into a struct
 *
 * @param msg The message to decode
 * @param configuration_version C-struct to decode the message contents into
 */
static inline void mavlink_msg_configuration_version_decode(const mavlink_message_t* msg, mavlink_configuration_version_t* configuration_version)
{
#if MAVLINK_NEED_BYTE_SWAP
	configuration_version->fw_version = mavlink_msg_configuration_version_get_fw_version(msg);
	configuration_version->port1 = mavlink_msg_configuration_version_get_port1(msg);
	configuration_version->port2 = mavlink_msg_configuration_version_get_port2(msg);
#else
	memcpy(configuration_version, _MAV_PAYLOAD(msg), MAVLINK_MSG_ID_CONFIGURATION_VERSION_LEN);
#endif
}

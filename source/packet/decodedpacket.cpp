#include "decodedpacket.h"

DecodedPacket::DecodedPacket(DecodedPacket::DecodeResult result, int errorLength, int8_t errorArray[], QByteArray decodedPacket){
    Q_ASSERT(errorLength <=2 && errorLength >=0);
    this->result_priv = result;
    this->errorLength_priv = errorLength;
    memcpy(errorArray_priv, errorArray, errorLength);
    this->decodedPacket_priv = decodedPacket;
}

int DecodedPacket::getErrorLength() const
{
    return this->errorLength_priv;
}

const int8_t *DecodedPacket::getErrorArray() const
{
    return this->errorArray_priv;
}

DecodedPacket::DecodeResult DecodedPacket::getResult() const
{
    return this->result_priv;
}

QByteArray DecodedPacket::getDecodedPacket() const
{
    return this->decodedPacket_priv;
}
